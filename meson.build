project(
    'desktop_pet',
    'c',
    version: '0.1',
    default_options: [
        'c_std=c23',
    ],
)

version_macro = '-DAPP_VERSION="v.' + meson.project_version() + '"'
add_project_arguments(version_macro, language: 'c')

app_sources = files(
    'src/main.c',
)

dependencies = []

target_os = host_machine.system()

if target_os == 'linux'
    add_project_arguments('-DPLATFORM_LINUX', language: 'c')
elif target_os == 'windows'
    add_project_arguments('-DPLATFORM_WINDOWS', language: 'c')
else
    error('Unsupported OS : ' + target_os)
endif

executable(
    'Desktop Pet',
    app_sources,
    dependencies: dependencies,
    include_directories: include_directories('src'),
)
