# Chroma

Is based on Chroma PC v2.5.0 as of this time

Colors!

## Background not visible
Is the background not visible (related to #26)? make sure all the environment effects are turned on (located under `Player Settings`).

### NOTE: This is a port of Chroma to the Quest, as such, it is in no way associated with Aeroluna. Please do not pester them about bugs, they are not associated with this port.

# TODO:
- [x] ~~Saber coloring is not working, no clue why~~ Fixed in commit [5709147](https://github.com/nyamimi/Chroma/commit/570914772f868e8a99338fbc1ddf18f0336daeaa)
- [x] Qosmetics support
- [x] ~~Chroma ring lights do not seem to work due to CJD not providing the customData or something else~~ Ring lights work though they are still unstable. Solution is ugly
- [x] ~~Light positions in maps such as `Madeon - The Prince` are not correct (assuming the [video from Tempex](https://www.youtube.com/watch?v=I0G34tNpbPU) is the reference)~~ Fixed in commit [2500809](https://github.com/nyamimi/Chroma/commit/2500809f1082134ee148b46c0744b303da583bf9)
- [x] Random crash due to retrieving [custom JSON data nullptr](https://github.com/nyamimi/Chroma/blob/c36d5fd48254008786a07c3a2419eac590fc2961/src/hooks/LightSwitchEventEffect.cpp#L57) weirdly
- [x] Track support (Requires Stack Track code)
- [x] ~~Bomb and obstacle coloring~~ Implemented in [1f41a9ca](https://github.com/nyamimi/Chroma/commit/1f41a9ca80a5f44d15e4aa08e17f8dbcd9ef07a1)
- [x] Add multiplayer scene hooks
- [x] Add `info.dat` environment removal support
- [x] CJD Gc bugs
- [x] Fix ChromaLite support
- [x] Merge 1.14 update and later commits:
  - [x] Do [LightID rework](https://github.com/Aeroluna/Chroma/commit/a8fc978b282af145c6ed263bfcce3485a31bb039) with rapidjson and somehow get json files in game
  - [x] Add noodle support
  - [x] Port over [these commits dear lord](https://github.com/BinaryElement/Chroma/compare/24452837a71867688a739a9f1297cd4b5efcaa77...Aeroluna:master) 
- [ ] Multiple tracks on one object    

- [ ] Optimize wherever possible

## Credits and Thank You

### Chroma, being one of the most requested Quest ports in Beat Saber's history, has gone through a long and incredible journey. So many people were a part of this project and long, confusing hours were spent polishing it. It's thanks to these people the project finally saw the light of day <3

#### Over 80 hours were poured into Chroma in just a week alone. Lots of love and effort went into making this as amazing and incredible as the original PC mod. It is incredible how far it's come, as we all used to think this mod was almost a joke or meme since no one was either motivated to make it or had the time to. Finally, it is here; Thank you:

* Sc2ad - Thank you so much for helping us fix annoying bugs and with your beautiful work in bs-hooks and custom-types. C++ 20 coroutines and safe pointers are amazing, and your advice went to finishing this in a properly good way.
* rui2015, Future, Caeden117, Auros, AeroLuna, Top_Cat - Thank you for clarifying, documenting and correcting us with how Chroma on PC works. Your help went a long way by taking us to the right direction.
* StackDoubleFlow - Collaborated with us by fixing CustomJSONData and correcting our mistakes. Thanks for Tracks and info.dat :D
* Pink - Gave us map suggestions/requirements
* RedBrumbler - Suggestions for adding compatibility with mods such as TrickSaber and Qosmetics
* Everyone who helped test the mod, support us and contribute. Your help pushed us to seeing this finally become a reality.

## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) 
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)
