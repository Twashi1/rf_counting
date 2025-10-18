#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

// LLVM command line option
static cl::opt<bool> SkipZeroReg(
    "skip-zero-reg", cl::desc("Skip counting RISC-V x0 register accesses"),
    cl::init(true));

// anonymous namespace so we don't clash with other passes
namespace {
struct CountRiscVRF : public MachineFunctionPass {
  static char ID;
  CountRiscVRF() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage& AU) const override {
    AU.addRequired<MachineBlockFrequencyInfo>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction& MF) override {
    const TargetRegisterInfo* TRI = MF.getSubtarget().getRegisterInfo();
    auto& MBFI = getAnalysis<MachineBlockFrequencyInfo>();

    // get target triple
    Triple TT(MF.getFunction().getParent()->getTargetTriple());
    bool IsRISCV = TT.isRISCV();

    double totalReads = 0.0, totalWrites = 0.0;
    // entry freq of function
    double entryFreq =
        static_cast<double>(MBFI.getBlockFreq(&MF.front()).getFrequency());

    errs() << "Running CountRiscVRF on function " << MF.getName() << "\n";

    // for each basic block
    //  count reads and writes on registers
    //  optionally exclude 0 register since not a real register
    for (auto& MBB : MF) {
      unsigned int reads = 0;
      unsigned int writes = 0;

      for (auto& MI : MBB) {
        for (auto& MO : MI.operands()) {
          if (!MO.isReg()) continue;

          Register R = MO.getReg();

          // optional register operands use special value 0 when not specified
          if (!R) continue;

          // ensure register is physical, and if so skip x0 for RISC-V
          if (R.isPhysical() && SkipZeroReg && IsRISCV) {
            // needs to be physical to get name
            const char* RegName = TRI->getName(R);
            if (RegName && (StringRef(RegName).equals_insensitive("x0") ||
                            StringRef(RegName).equals_insensitive("zero")))
              continue;
          }

          if (MO.isUse()) {
            ++reads;
          }

          if (MO.isDef()) {
            ++writes;
          }
        }
      }

      // freq of basic block
      double freq = static_cast<double>(MBFI.getBlockFreq(&MBB).getFrequency());
      // relative frequency to function entries
      double normFreq = entryFreq ? freq / entryFreq : 1.0;

      errs() << "  MBB#" << MBB.getNumber() << " : reads=" << reads
             << " writes=" << writes << " freq=" << normFreq << "\n";

      totalReads += reads * normFreq;
      totalWrites += writes * normFreq;
    }

    errs() << "  ==> " << MF.getName() << " estimated RF reads=" << totalReads
           << " writes=" << totalWrites << "\n\n";

    return false;  // does not modify the MachineFunction
  }
};
}  // namespace

char CountRiscVRF::ID = 0;

// Register with the legacy pass manager (this is what llc uses)
static RegisterPass<CountRiscVRF> X(
    "count-riscv-rf", "Count register-file accesses (RISC-V, LLVM 17)", false,
    false);
