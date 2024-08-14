#include "clang/Driver/Compilation.h"
#include "clang/Driver/Driver.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "llvm/Option/OptTable.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "llvm/ADT/ArrayRef.h"

#include <memory>

using namespace clang;
using namespace clang::driver;

int main(int argc, const char **argv) {
    llvm::InitLLVM X(argc, argv);

    // Initialize targets and diagnostic printers
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    
    // Create a diagnostic engine
    llvm::IntrusiveRefCntPtr<DiagnosticOptions> diagOpts = new DiagnosticOptions();
    TextDiagnosticPrinter *diagClient = new TextDiagnosticPrinter(llvm::errs(), &*diagOpts);
    IntrusiveRefCntPtr<DiagnosticIDs> DiagID(new DiagnosticIDs());
    IntrusiveRefCntPtr<DiagnosticsEngine> diags = new DiagnosticsEngine(DiagID, &*diagOpts, diagClient);

    // Initialize the Clang driver
    Driver TheDriver(argv[0], llvm::sys::getDefaultTargetTriple(), *diags);

    // Parse the command line arguments
    std::vector<const char *> Args(argv + 1, argv + argc);
    // We don't actually care if the command
    Args.push_back("-fsyntax-only");

    std::unique_ptr<Compilation> C(TheDriver.BuildCompilation(Args));

    // Iterate over the jobs in the compilation
    for (const auto &Job : C->getJobs()) {
        for (const auto &Input : Job.getInputInfos()) {
            // llvm::outs() << "Source file: " << Input.getAsString() << "\n";
            std::string FilenameWithQuotes = Input.getAsString();
            llvm::outs() << FilenameWithQuotes.substr(1, FilenameWithQuotes.size() - 2) << "\n";
        }
    }

    return 0;
}
