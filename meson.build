project(
    'desktop_pet',
    'c',
    version: '0.1',
    default_options: [
        'c_std=c99',
    ],
)

version_macro = '-DAPP_VERSION="v.' + meson.project_version() + '"'
add_project_arguments(version_macro, language: 'c')

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

app_sources = files(
    'src/main.c',
    'src/window/linux_window.c',
    'src/window/windows_window.c',
    'src/renderer/renderer.c',
    'src/physics/physics.c',
    'src/tray/linux_tray.c',
    'src/tray/win_tray.c',
)

dependencies = [m_dep]

target_os = host_machine.system()

if target_os == 'windows'
    add_project_arguments('-DPLATFORM_WINDOWS', language: 'c')
    dependencies += [
        cc.find_library('user32'),
        cc.find_library('gdi32'),
        cc.find_library('shell32'),
    ]
elif target_os == 'linux'
    add_project_arguments('-DPLATFORM_LINUX', language: 'c')
    dependencies += [
        dependency('x11'),
        dependency('xext'),
        dependency('gtk+-3.0'),
    ]
endif

executable(
    'Desktop Pet',
    app_sources,
    dependencies: dependencies,
    include_directories: include_directories('src'),
)
