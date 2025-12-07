#include "spef_actions.hpp"
#include "spef_random.hpp"
#include "spef_structs.hpp"
#include "spef_write.hpp"
#include <CLI/CLI.hpp>
#include <filesystem>
#include <fstream>
#include <print>
#include <tao/pegtl/contrib/analyze.hpp>
//#include <tao/pegtl/contrib/trace.hpp>

namespace pegtl = tao::pegtl;
namespace fs = std::filesystem;

int main(int argc, char const *const *argv) {
  if (pegtl::analyze<spef_grammar>() != 0) {
    std::println(stderr, "cycles without progress detected!");
    return 1;
  }

  CLI::App app{"A tool to parse and check the correctness of a SPEF file"};

  std::string input_file;
  std::string output_file;
  std::size_t random_count = 0;

  app.add_option("input", input_file, "Input SPEF file to parse")
      ->check(CLI::ExistingFile);
  app.add_option("-o,--output", output_file, "Output file (default: stdout)");
  app.add_option(
      "--random",
      random_count,
      "Generate N random SPEF files for testing");

  CLI11_PARSE(app, argc, argv);

  if (random_count > 0) {
    for (std::size_t i = 0; i < random_count; ++i) {
      RandomSPEFGenerator rsg(std::format("Random {}", i), 10, 10, 100);
      SPEF spef = rsg.generate();

      fs::path const outfile_p{std::format("random/random_{:02d}.spef", i)};
      fs::create_directories(outfile_p.parent_path());
      {
        std::ofstream outfile(outfile_p);
        outfile << spef;
      }
    }
    return 0;
  }

  if (input_file.empty()) {
    std::println(stderr, "Error: Input file is required");
    return 1;
  }

  std::filesystem::path const spef_file{input_file};

  bool success = false;

  // outer try/catch for normal exceptions that might occur for example if the
  // file is not found
  try {
    pegtl::mmap_input input{spef_file};

    // inner try/catch for the parser exceptions
    try {
      //pegtl::tracer<pegtl::tracer_traits<>> tracer(input);
      //tracer.parse<spef_grammar>(input);
      SPEF spef;
      SPEFHelper spef_h;
      success = pegtl::parse<pegtl::must<spef_grammar>, spef_action>(
          input,
          spef,
          spef_h);

      if (!output_file.empty()) {
        std::ofstream outfile(output_file);
        if (!outfile) {
          std::println(
              stderr,
              "Error: Cannot open output file '{}'",
              output_file);
          return 1;
        }
        outfile << spef;
      } else {
        std::cout << spef;
      }
    } catch (pegtl::parse_error &err) {
      std::println(stderr, "ERROR: An exception occurred during parsing:");
      // this catch block needs access to the input
      auto const &pos = err.positions().front();
      std::println(
          stderr,
          "{}\n{}\n{: >{}}",
          err.what(),
          input.line_at(pos),
          pos.column,
          '^');
    }
  } catch (std::exception const &e) {
    std::println(stderr, "{}", e.what());
  }

  if (!success) {
    std::println(stderr, "Parsing failed");
    return 2;
  }

  return 0;
}
