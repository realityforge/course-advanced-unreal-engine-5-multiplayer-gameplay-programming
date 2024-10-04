#!/usr/bin/env sh

braid push Plugins/RuleRanger && git add . && git stash && braid up Plugins/RuleRanger && git stash pop && git reset
