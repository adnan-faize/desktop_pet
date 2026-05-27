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
    'src/*',
)

dependencies = []

# target_cpu = host_machine.cpu_family()

# if target_cpu == 'arm'

# endif

target_os = host_machine.system()

if target_os == 'windows'
    add_project_arguments('-DPLATFORM_WINDOWS', language: 'c')

    user32_dependency = meson.get_compiler('c').find_library('user32')
    gdi32_dependency = meson.get_compiler('c').find_library('user32')

    dependencies += [
        user32_dependency,
        gdi32_dependency,
    ]

elif target_os == 'linux'
    add_project_arguments('-DPLATFORM_LINUX', language: 'c')

    x11_dependency = dependency('x11', required: true)
    xfixes_dependency = dependency('xfixes', required: true)

    dependencies += [
        x11_dependency,
        xfixes_dependency,
    ]

    wayland_dependency = dependency('wayland-client', required: false)
    if wayland_dependency.found()
        dependencies += [wayland_dependency]
        add_project_arguments('-DWAYLAND', language: 'c')
    endif
endif

executable(
    'Desktop Pet',
    app_sources,
    dependencies: dependencies,
)
