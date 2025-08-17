project('Renderer', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++17'])

# 可选：添加调试信息（必须在构建目标之前）
if get_option('buildtype') == 'debug'
    add_project_arguments('-DDEBUG', language : 'cpp')
endif

# 查找系统依赖
opengl_dep = dependency('OpenGL', required : true)
glfw_dep = dependency('glfw3', required : true)

# GLM 是 header-only 库，可以通过 pkg-config 或者 subproject 方式获取
glm_dep = dependency('glm', required : false)

# 如果系统没有 GLM，可以使用 fallback 到 subproject
if not glm_dep.found()
    glm_proj = subproject('glm')
    glm_dep = glm_proj.get_variable('glm_dep')
endif

# 收集所有源文件
sources = [
    'main.cpp'
]

tsrm_sources = [
    'tsrm_demo.cpp'
]

# 创建可执行文件
Renderer = executable('Renderer', 
    sources,
    dependencies : [opengl_dep, glfw_dep, glm_dep],
    install : true)

# TSRM 矩阵学习 Demo
TSRM_Demo = executable('TSRM_Demo', 
    tsrm_sources,
    dependencies : [opengl_dep, glfw_dep, glm_dep],
    install : true)