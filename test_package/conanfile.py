from conans import ConanFile
import os

channel = os.getenv("CONAN_CHANNEL", "testing")
username = os.getenv("CONAN_USERNAME", "xylosper")

class B0UtilTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "b0util/0.0.1@xylosper/stable"

    def imports(self):
        self.copy("*.dll", "bin", "bin")
        self.copy("tests.exe", "bin", "bin")

    def test(self):
        os.chdir("bin")
        #self.run("dir %s" % self.conanfile_directory)
        self.run(".%stests" % os.sep)
