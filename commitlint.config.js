module.exports =
{
    extends: ['@commitlint/config-conventional'],
    rules:
    {
        'scope-enum':
        [
            2, 'always',
            [
                'core', 'file_types', 'gui', 'minsynth', 'render', 'tests', 'window',
                'editor', 'runner', 'painter', 'pixel', 't3d'
            ]
        ],
        'type-enum': [2, 'always', ['fix', 'feat', 'chore', 'docs', 'style', 'refactor', 'perf', 'test', 'revert', 'ci', 'build']]
    }
};
