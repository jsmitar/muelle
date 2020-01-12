#!/usr/bin/env sh

/usr/bin/node "$1/node_modules/typescript/lib/tsserver.js" --syntaxOnly --useInferredProjectPerProjectRoot --disableAutomaticTypingAcquisition
