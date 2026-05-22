#include "src/resource_core/resource_core.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <thread>
#include <vector>

using namespace lab4::resource;

std::string temp_file()
{
    static int counter = 0;
    return "test_" + std::to_string(counter++) + ".tmp";
}

void delete_file(const std::string& path)
{
    std::remove(path.c_str());
}

TEST_CASE("File auto-closes when destroyed")
{
    std::string path = temp_file();

    {
        FileHandle fh(path);
        fh.write("Hello");
        REQUIRE(fh.is_open() == true);
    }

    FileHandle fh2(path);
    REQUIRE(fh2.read() == "Hello");

    delete_file(path);
}

TEST_CASE("FileHandle can be moved, not copied")
{
    std::string path = temp_file();
    FileHandle fh1(path);
    fh1.write("data");

    FileHandle fh2 = std::move(fh1);
    REQUIRE(fh1.is_open() == false);
    REQUIRE(fh2.is_open() == true);
    REQUIRE(fh2.read() == "data");

    delete_file(path);
}

TEST_CASE("Opening bad file throws exception")
{
    REQUIRE_THROWS_AS(FileHandle("/bad/path/file.txt"), ResourceError);
}

TEST_CASE("ResourceManager returns same object for same file")
{
    std::string path = temp_file();
    auto& rm = ResourceManager::instance();

    auto r1 = rm.get_resource(path);
    auto r2 = rm.get_resource(path);

    REQUIRE(r1.get() == r2.get());

    delete_file(path);
}

TEST_CASE("ResourceManager cache cleanup")
{
    std::string path = temp_file();
    auto& rm = ResourceManager::instance();

    {
        auto r = rm.get_resource(path);
        REQUIRE(rm.cache_size() == 1);
    }

    REQUIRE(rm.cache_size() == 1);

    rm.cleanup();
    REQUIRE(rm.cache_size() == 0);

    delete_file(path);
}

TEST_CASE("Multiple threads can use same file")
{
    std::string path = temp_file();
    auto& rm = ResourceManager::instance();

    std::vector<std::thread> threads;

    for (int i = 0; i < 5; ++i)
    {
        threads.push_back(std::thread([&rm, path, i]() {
            auto h = rm.get_resource(path);
            h->write("thread " + std::to_string(i) + "\n");
        }));
    }

    for (auto& t : threads)
    {
        t.join();
    }

    auto h = rm.get_resource(path);
    std::string content = h->read();
    REQUIRE(content.find("thread") != std::string::npos);

    delete_file(path);
}

TEST_CASE("Write and read work correctly")
{
    std::string path = temp_file();
    FileHandle fh(path);

    fh.write("Line 1\n");
    fh.write("Line 2\n");

    std::string content = fh.read();
    REQUIRE(content.find("Line 1") != std::string::npos);
    REQUIRE(content.find("Line 2") != std::string::npos);

    delete_file(path);
}