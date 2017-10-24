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
    description = "utils"

    def requirements(self):
        self.requires("catch/[>=1.10]@xylosper/stable")
        self.requires("spdlog/[>=0.14]@xylosper/stable")
        self.requires("fmt/[>=4.0]@xylosper/stable")

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
                self.run_msvc("jom qmake_all")
                self.run_msvc("jom -j%d" % tools.cpu_count())

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
