# Third-party components

- CommonLibSSE-NG 10.0.1: GPL-3.0-or-later with the Modding Exception and GPL-3.0 Linking Exception (with Corresponding Source). See `CommonLib-COPYING.txt` and `CommonLib-EXCEPTIONS.md` in release packages. Source: https://github.com/alandtse/CommonLibSSE-NG/tree/v10.0.1
- SKSE: https://skse.silverlock.org/
- spdlog 1.16.0: MIT License, https://github.com/gabime/spdlog
- fmt 12.1.0: MIT License, https://github.com/fmtlib/fmt
- rapidcsv 8.90: BSD 3-Clause License, https://github.com/d99kris/rapidcsv
- DirectX Tool Kit 2025-10-27: MIT License, https://github.com/microsoft/DirectXTK
- DirectXMath 2025-04-03: MIT License, https://github.com/microsoft/DirectXMath
- MinHook hde64 1.3.4: BSD notices, https://github.com/TsudaKageyu/minhook

The original plugin source remains MIT-licensed. The source corresponding to the published 1.1.1 DLL is available at https://github.com/dickmna/DynamicUnrelentingForce, together with the matching dependencies in `dependencies/`. Player archives contain license notices and a link to the corresponding GitHub source; they do not include source trees or debug symbols.

`dependencies/REBUILD.md` records the exact versions, flags, and rebuilding steps. The dependency sources and their upstream licenses are preserved from the published release archive. `dependencies/SHA256SUMS.txt` retains the hashes of the upstream archives from which these already-expanded trees were obtained. `SOURCE_PROVENANCE.json` records the hashes of the published files and their repository paths.
