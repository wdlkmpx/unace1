project('unace1',
	'c',
	version: '1.7w'
)

add_project_arguments('-DHAVE_CONFIG_H=1', language: 'c')

cc = meson.get_compiler('c')
config = configuration_data()

config.set_quoted('VERSION', meson.project_version())
config.set_quoted('PACKAGE_PREFIX', get_option('prefix'))

configure_file(output : 'config.h',
               configuration : config)

#===========================================================================

#subdir('src')

cfiles = files(
	'src/uac.c',
	'src/uac_dcpr.c',
	'src/unace.c',
	)

unace1 = executable('unace1',
	cfiles,
	include_directories: ['.', 'src', 'lib'],
	#c_args: ['-DHAVE_CONFIG_H'],
	install: true)

#===========================================================================
#tests

## broken paths in meson..
#run_tests_sh = find_program('scripts/run-tests.sh', required: true)
#test('test script', run_tests_sh, workdir: meson.global_source_root())

tests = {
	'dirtraversal1.ace': true,
	'dirtraversal2.ace': true, 
	'onefile.ace': false,
#	'passwd.ace': false,
	'ZGFX2.ace': false,
	'zdir.ace': false,
	'multivolume.ace': false,
	'ALCHT.ACE': false,
	'Patch.ace': false,
	'trps.ace': false,
#	'karate.ace': false,
}

foreach name, should_fail: tests
    test('test',
         unace1,
         args: ['x', '-y', '../tests/@0@'.format(name)],
         should_fail: should_fail
    )
endforeach

#test('test', unace1, args: ['x', '-y', '../tests/dirtraversal1.ace'], should_fail: true)
#test('test', unace1, args: ['x', '-y', '../tests/dirtraversal2.ace'], should_fail: true)
#test('test', unace1, args: ['x', '-y', '../tests/onefile.ace'])
##test('test', unace1, args: ['x', '-y', '../tests/passwd.ace'])
#test('test', unace1, args: ['x', '-y', '../tests/ZGFX2.ace'])
#test('test', unace1, args: ['x', '-y', '../tests/zdir.ace'])
#test('test', unace1, args: ['x', '-y', '../tests/multivolume.ace'])
#test('test', unace1, args: ['x', '-y', '../tests/ALCHT.ACE'])
#test('test', unace1, args: ['x', '-y', '../tests/Patch.ace'])
#test('test', unace1, args: ['x', '-y', '../tests/trps.ace'])
##test('test', unace1, args: ['x', '-y', '../tests/karate.ace'])
