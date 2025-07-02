#!/usr/bin/env python3
"""
A script to run clang-tidy on C++ projects.
This script detects C++ files in a directory and runs clang-tidy with
configurable checks to find and potentially fix code issues.
"""

import argparse
import fnmatch
import json
import os
import subprocess
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed


def find_compile_commands(directory):
    """
    Try to find compile_commands.json in the directory or its parents.
    This is the database file that clang-tidy uses for compilation settings.
    """
    # current_dir = os
    # while current_dir != os.path.dirname(current_dir):  # Stop at filesystem root
    #     compile_commands = os.path.join(current_dir, 'compile_commands.json')
    #     if os.path.isfile(compile_commands):
    #         return compile_commands
    #     current_dir = os.path.dirname(current_dir)
    return os.path.join(".", "build", "compile_commands.json")


def is_cpp_file(filename):
    """Check if the file is a C++ source file."""
    cpp_extensions = ['.cpp', '.cc', '.cxx', '.c++', '.c']
    ext = os.path.splitext(filename)[1].lower()
    return ext in cpp_extensions


def is_header_file(filename):
    """Check if the file is a C++ header file."""
    header_extensions = ['.h', '.hpp', '.hxx', '.h++', '.hh']
    ext = os.path.splitext(filename)[1].lower()
    return ext in header_extensions


def match_file(filename, exclude_patterns):
    """Return True if file is a C++ file or a directory."""
    base_name = os.path.basename(filename)

    if base_name.startswith('.'):
        return False

    for pattern in exclude_patterns:
        if fnmatch.fnmatch(base_name, pattern):
            return False

    if is_header_file(filename):
        return True

    if is_cpp_file(filename):
        return True

    if os.path.isdir(filename):
        return True

    return False


def find_files(filenames, exclude_patterns):
    """Yield filenames."""
    while filenames:
        name = filenames.pop(0)
        if os.path.isdir(name):
            for root, directories, children in os.walk(name):
                filenames += [os.path.join(root, f) for f in sorted(children)
                              if match_file(os.path.join(root, f),
                                            exclude_patterns)]
                directories[:] = [d for d in directories
                                  if match_file(os.path.join(root, d),
                                                exclude_patterns)]
        else:
            yield name


def run_clang_tidy(filename, args):
    """Run clang-tidy on a single file."""
    cmd = [args.clang_tidy_path, '--config-file', './.clang-tidy', filename]
    
    # Add checks\
    # Add compile commands database if available
    if args.compile_commands:
        cmd.append(f'-p={args.compile_commands}')
    
    # Add fix option if requested
    if args.fix:
        cmd.append('--fix')
        cmd.append('--fix-errors')
        cmd.append('--fix-notes')
    
    # Add additional clang-tidy options
    if args.extra_args:
        cmd.extend(args.extra_args)
    
    cmd.append("--quiet")

    try:
        print("Command is ", cmd)
        result = subprocess.run(cmd, capture_output=True, text=True, check=False)
        return filename, result.returncode, result.stdout, result.stderr
    except subprocess.SubprocessError as e:
        return filename, 1, "", f"Error running clang-tidy: {str(e)}"


def main():
    parser = argparse.ArgumentParser(description='Run clang-tidy on C++ files.')
    parser.add_argument('files', nargs='+', help='Files or directories to process')
    parser.add_argument('--clang-tidy-path', default='clang-tidy',
                        help='Path to clang-tidy executable')
    parser.add_argument('--compile-commands', 
                        help='Path to compile_commands.json (automatically detected if not specified)')
    parser.add_argument('--checks', 
                        default='bugprone-*,cppcoreguidelines-*,modernize-*,performance-*,readability-*,-modernize-use-trailing-return-type',
                        help='Comma-separated list of checks to enable')
    parser.add_argument('--fix', action='store_true',
                        help='Apply suggested fixes')
    parser.add_argument('--header-filter', default='.*',
                        help='Regular expression to filter headers')
    parser.add_argument('--exclude', action='append',
                        dest='exclude_patterns', default=[], metavar='pattern',
                        help='Exclude files matching this pattern')
    parser.add_argument('--extra-args', action='append', default=[],
                        help='Extra arguments to pass to clang-tidy')
    parser.add_argument('--jobs', '-j', type=int, default=os.cpu_count(),
                        help='Number of parallel jobs')
    parser.add_argument('--verbose', action='store_true',
                        help='Print verbose messages')
    parser.add_argument('--export-fixes', 
                        help='YAML file to store suggested fixes in')
    
    args = parser.parse_args()

    # For Python 2 compatibility
    args.files = [filename.decode(sys.getfilesystemencoding())
                  if hasattr(filename, 'decode') else filename
                  for filename in args.files]

    # Auto-detect compile_commands.json if not specified
    if not args.compile_commands:
        for file_path in args.files:
            if os.path.isdir(file_path):
                compile_commands = find_compile_commands(file_path)
                if compile_commands:
                    args.compile_commands = compile_commands
                    if args.verbose:
                        print(f"Found compile_commands.json at {compile_commands}")
                    break

    # Filter to only C++ source files
    cpp_files = []
    for filename in sorted(find_files(args.files, exclude_patterns=args.exclude_patterns)):
        if is_cpp_file(filename) or is_header_file(filename):
            cpp_files.append(filename)
            if args.verbose:
                print(f"Will process {filename}")

    if not cpp_files:
        print("No C++ files found to process.")
        return 0

    print(f"Processing {len(cpp_files)} files with clang-tidy...")
    
    # Process files in parallel
    status = 0
    with ProcessPoolExecutor(max_workers=args.jobs) as executor:
        futures = {executor.submit(run_clang_tidy, filename, args): filename for filename in cpp_files}
        
        for future in as_completed(futures):
            filename, return_code, stdout, stderr = future.result()
            if return_code != 0:
                status = 1
            
            if stdout or stderr:
                print(f"\n--- {filename} ---")
                if stdout:
                    print(stdout)
                if stderr:
                    print(stderr, file=sys.stderr)

    if status == 0:
        print("\nAll files processed successfully! No issues found.")
    else:
        print("\nIssues were found in one or more files.")
        
    return status


if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        sys.exit(1)
