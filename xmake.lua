local project_name = "rtsyn-thread"
local project_xmake_repo = "rtsyn-xmake-repo"

set_license("GPL-3.0-or-later")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")
set_defaultmode("release")
if is_mode("release") then
	set_optimize("fastest")
	set_strip("all")
	set_symbols("hidden")
end

option("valgrind", { default = false, description = "Run tests with Valgrind" })
option("tests", { default = true, description = "Build tests" })

option("thread_core")
set_default("posix")
set_values("posix", "preempt_rt", "xenomai")
set_showmenu(true)
set_description("Thread core backend", "  - posix", "  - preempt_rt", "  - xenomai")
option_end()

set_languages("c23")
if has_config("tests") then
	add_requires("gtest")
	add_requires("rtsyn-test-utils")
end
add_requires("rtsyn-defaults")

local workspace = os.getenv("RTSYN_WORKSPACE")
if workspace then
	local repository_dir = path.join(workspace, project_xmake_repo)
	add_repositories(project_xmake_repo .. " " .. repository_dir)
else
	add_repositories(project_xmake_repo .. " https://github.com/rtsyn-developers/" .. project_xmake_repo .. ".git")
end

target(project_name)
set_kind("static")
add_files("src/*.c")
add_files("src/thread/core/common.c")
add_defines("_GNU_SOURCE")
add_packages("rtsyn-defaults")
add_includedirs("include", { public = true })
add_includedirs("src")
add_headerfiles("include/(rtsyn/**.h)")
on_load(function(target)
	local backend = get_config("thread_core") or "posix"
	target:add("files", path.join("src", "thread", "core", backend .. ".c"))
end)

local rtsyn_modules = {
	{ name = "thread", path = "thread" },
}

if has_config("tests") then
	for _, rtsyn_module in ipairs(rtsyn_modules) do
		local tests_name = "tests/" .. rtsyn_module.path .. "-tests"
		target(tests_name)
		set_kind("binary")
		if has_config("valgrind") then
			add_rules("@rtsyn-test-utils/valgrind")
		end
		add_deps(project_name)
		add_packages("gtest")
		add_links("gtest_main")
		add_includedirs("src")
		add_files("tests/" .. rtsyn_module.path .. ".cpp")
		add_tests(rtsyn_module.name)
	end
end
--
-- If you want to known more usage about xmake, please see https://xmake.io
--
