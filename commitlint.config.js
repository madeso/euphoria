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
                'editor', 'engine', 'painter', 'pixel', 't3d'
            ]
        ],
        'type-enum': [2, 'always', ['feat', 'fix', 'docs', 'style', 'refactor', 'test', 'revert']]
    }
};
