#compdef kpm

_kpm() {
    local line
    _arguments -C \
        "1: :((init\:"Initialize project"
               get\:"Download package"
               list\:"List packages"
               remove\:"Delete package"
               tidy\:"Sync dependencies"
               help\:"Show help"))" \
        "*::arg:->args"
}

_kpm "$@"

