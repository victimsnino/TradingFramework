from conan import ConanFile


class Recipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("grpc/1.54.3", transitive_libs=True, transitive_headers=True)
        self.requires("fmt/11.0.2")
        self.requires("protobuf/3.21.12")
        self.requires("tgbot/1.7.2")

    def build_requirements(self):
        self.test_requires("catch2/3.6.0")
