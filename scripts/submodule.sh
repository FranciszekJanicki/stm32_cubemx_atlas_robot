#!/bin/bash
set -e

SUBMODULE_URL="$1"
SUBMODULE_DIR="$2"
SUBMODULE_NAME="$3"
SUBMODULE_ACTION="$4"
SUBMODULE_PATH="$DIR/$NAME"

function log_skip {
    echo "Skipping $1: $SUBMODULE_NAME"
}

function add_submodule {
    if [ -d "$SUBMODULE_PATH" ]; then
        log_skip "add (already exists)"
        return
    fi

    echo "Adding submodule $SUBMODULE_NAME → $SUBMODULE_PATH"
    git submodule add -f "$SUBMODULE_URL" "$SUBMODULE_PATH"
}

function remove_submodule {
    if [ ! -d "$SUBMODULE_PATH" ]; then
        log_skip "remove (not found)"
        return
    fi

    echo "Removing submodule $SUBMODULE_NAME"
    git submodule deinit -f "$SUBMODULE_PATH"
    git rm -rf "$SUBMODULE_PATH"
    rm -rf ".git/modules/$SUBMODULE_PATH"
    rm -rf "$SUBMODULE_PATH"
}

function update_submodule {
    if [ ! -d "$SUBMODULE_PATH" ]; then
        log_skip "update (not found)"
        return
    fi

    echo "Updating submodule $SUBMODULE_NAME"
    git submodule update --remote "$SUBMODULE_PATH"
}

if [ "$SUBMODULE_ACTION" = "add" ]; then
    add_submodule "$SUBMODULE_URL" "$SUBMODULE_DIR" "$SUBMODULE_NAME"
elif [ "$SUBMODULE_ACTION" = "remove" ]; then
    remove_submodule "$SUBMODULE_DIR" "$SUBMODULE_NAME"
elif [ "$SUBMODULE_ACTION" = "update" ]; then
    update_submodule "$SUBMODULE_DIR" "$SUBMODULE_NAME"
else
    echo "Invalid action: $SUBMODULE_ACTION"
    exit 1
fi
