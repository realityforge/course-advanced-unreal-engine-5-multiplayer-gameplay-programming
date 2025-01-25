#!/usr/bin/env sh

braid push Plugins/RuleRanger && braid push Plugins/ModularGameplayActors && braid push Plugins/ModularGasGameplayActors && git add . && git stash && braid up Plugins/Aeon && braid up Plugins/RuleRanger && braid up Plugins/ModularGameplayActors && braid up Plugins/ModularGasGameplayActors && git stash pop && git reset
