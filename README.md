# Chroma

Core Chroma Mod for Quest

### NOTE: This is a port of Chroma to the Quest, as such, it is in no way associated with Aeroluna. Please do not pester them about bugs, they are not associated with this port.

# TODO:
- [x] ~~Saber coloring is not working, no clue why~~ Fixed in commit [5709147](https://github.com/nyamimi/Chroma/commit/570914772f868e8a99338fbc1ddf18f0336daeaa)
- [ ] Qosmetics support
- [x] ~~Chroma ring lights do not seem to work due to CJD not providing the customData or something else~~ Ring lights work though they are still unstable. Solution is ugly
- [x] ~~Light positions in maps such as `Madeon - The Prince` are not correct (assuming the [video from Tempex](https://www.youtube.com/watch?v=I0G34tNpbPU) is the reference)~~ Fixed in commit [2500809](https://github.com/nyamimi/Chroma/commit/2500809f1082134ee148b46c0744b303da583bf9)
- [ ] Random crash due to retrieving [custom JSON data nullptr](https://github.com/nyamimi/Chroma/blob/c36d5fd48254008786a07c3a2419eac590fc2961/src/hooks/LightSwitchEventEffect.cpp#L57) weirdly
- [ ] Noodle Support
- [x] ~~Bomb and obstacle coloring~~ Implemented in [1f41a9ca](https://github.com/nyamimi/Chroma/commit/1f41a9ca80a5f44d15e4aa08e17f8dbcd9ef07a1)
- [ ] Add multiplayer scene hooks

## Credits and thank you

## Credits

* [zoller27osu](https://github.com/zoller27osu), [Sc2ad](https://github.com/Sc2ad) and [jakibaki](https://github.com/jakibaki) - [beatsaber-hook](https://github.com/sc2ad/beatsaber-hook)
* [raftario](https://github.com/raftario) 
* [Lauriethefish](https://github.com/Lauriethefish) and [danrouse](https://github.com/danrouse) for [this template](https://github.com/Lauriethefish/quest-mod-template)