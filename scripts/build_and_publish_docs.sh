#!/usr/bin/env bash

cd $(git rev-parse --show-toplevel)
git branch -f gh-pages main
git switch gh-pages
doxygen
git add docs
git commit --no-gpg-sign -m "Build and publish documentation"
git push -f origin gh-pages
git switch main