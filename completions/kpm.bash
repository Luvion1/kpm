_kpm_completions() {
    local cur prev opts
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="init get list remove tidy help"

    case "$prev" in
        get|remove)
            # Future: could list installed packages here
            return 0
            ;;
    esac

    COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
    return 0
}
complete -F _kpm_completions kpm
