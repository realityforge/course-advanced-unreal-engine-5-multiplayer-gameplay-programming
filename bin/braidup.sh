#!/usr/bin/env sh

braid push Plugins/Aeon && braid push Plugins/ModularGameplayActors && braid push Plugins/ModularGasGameplayActors && braid push Plugins/RuleRanger && git add . && git stash && braid up Plugins/Aeon && braid up Plugins/ModularGameplayActors && braid up Plugins/ModularGasGameplayActors && braid up Plugins/RuleRanger && braid up Plugins/MDMetaDataEditor && git stash pop && git reset
