### Rewards service public facing API changelog

---
### New API (16th September 2019)
##### Related PR

[https://github.com/brave/brave-core/pull/3444](https://github.com/brave/brave-core/pull/3444)

##### Description

We added new API which allows us to get information if we should only show anonymous wallet functionality.
It's based on user location, country. List of countries is located in `static_values.h`. 
We use [Chromium api](https://cs.chromium.org/chromium/src/components/country_codes/country_codes.h?type=cs&q=GetCountryIDFromPrefs&g=0&l=53)
to determinate which country user is in.

##### Change

| | Old version | New version |
|---|---|---|
|  Signature    |    | `bool OnlyAnonWallet()`  |
|  Return value |    | `true` or `false`        |
|  Parameters   |    |                          |

---