from conans import ConanFile, tools

class B0UtilConan(ConanFile):
    name    = "b0util"
    url     = "http://github.com/xylosper/b0util"
    version = "latest"
    license = "private" 
    settings = "os", "compiler", "build_type", "arch"
    generators = "qmake"
    build_policy = "missing"
    exports_sources = "*.pro", "src/*"
    
    def requirements(self):
        self.requires("catch/latest@xylosper/stable")
        self.requires("spdlog/latest@xylosper/stable")
        self.requires("fmt/latest@xylosper/stable")

    #def imports(self):
        #self.copy("*.dll", dst="bin", src="bin") # From bin to bin
        #self.copy("*.dylib*", dst="bin", src="lib") # From lib to bin

    def run_msvc(self, cmd):
        vcvars = tools.vcvars_command(self.settings)
        self.run("%s && %s" % (vcvars, cmd))

    def build(self):
        bs = str(self.settings.build_type).lower()
        with tools.chdir("./src"):
            self.run_msvc("qmake -spec win32-msvc CONFIG+=no_test CONFIG+=%s b0util.pro" % bs)
            if self.settings.os == "Windows":
                vcvars = tools.vcvars_command(self.settings)
                self.run_msvc("jom qmake_all")
                self.run_msvc("jom -j%d" % tools.cpu_count())
        #cmake = CMake(self.settings)
        #flags = "-DBUILD_SHARED_LIBS=TRUE -DFMT_TEST=FALSE -DFMT_INSTALL=TRUE -DFMT_DOCS=FALSE"
        #flags += " -DCMAKE_INSTALL_PREFIX=\"%s\"" % self.package_folder
        #if self.settings.os != "Windows":
            #flags += " -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE"
        #self.run("cmake %s/%s-%s %s %s" % (self.conanfile_directory, self.name, self.version, cmake.command_line, flags))
        #self.run("cmake --build . --target install %s" % cmake.build_config)

    def package(self):
        bt = str(self.settings.build_type).lower()
        self.copy("*.hpp", dst="include/b0util", src="src/b0util", keep_path=True)
        self.copy("*.exe", dst="bin", src="build/%s/bin" % bt, keep_path=True)
        self.copy("*.dll", dst="bin", src="build/%s/bin" % bt, keep_path=True)
        self.copy("*.lib", dst="lib", src="build/%s/lib" % bt, keep_path=True)
        self.copy("*.dylib", dst="lib", src="build/%s/lib" % bt, keep_path=True)
        self.copy("*.so", dst="lib", src="build/%s/lib" % bt, keep_path=True)
        self.copy("*.a", dst="lib", src="build/%s/lib" % bt, keep_path=True)

    def package_info(self):
        self.cpp_info.defines.append("B0UTIL_SHARED")
        self.cpp_info.libs.append("b0util")
