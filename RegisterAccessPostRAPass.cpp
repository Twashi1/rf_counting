#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct RegisterAccessPostRAPass : public MachineFunctionPass {
  static char ID;
  RegisterAccessPostRAPass() : MachineFunctionPass(ID) {}

  // Need machine block frequency info
  void getAnalysisUsage(AnalysisUsage& usage) const override {
    usage.addRequired<MachineBlockFrequencyInfoWrapperPass>();
    usage.setPreservesAll();
  }

  // bool doFinalization(Module& module) override { return false; }

  bool runOnMachineFunction(MachineFunction& machineFunction) override {
    // TODO: information is not available because its discarded? we need to run
    // full pipeline, maybe just need to build in-tree for full information
    errs() << "Hello\n";
    MachineBlockFrequencyInfoWrapperPass* machineBlockFrequencyInfoWrapper =
        getAnalysisIfAvailable<MachineBlockFrequencyInfoWrapperPass>();
    MachineBlockFrequencyInfo* machineBlockFrequencyInfo = nullptr;

    if (machineBlockFrequencyInfoWrapper) {
      machineBlockFrequencyInfo = &machineBlockFrequencyInfoWrapper->getMBFI();
    } else {
      errs() << "Couldn't get machine block frequency info\n";
    }

    size_t reads = 0;
    size_t writes = 0;
    size_t numLoads = 0;
    size_t numStores = 0;

    errs() << "[RegAccessPostRA] " << machineFunction.getName() << "\n";

    for (auto& machineBasicBlock : machineFunction) {
      // TODO: null pointer, not properly getting machine block frequency info?
      if (machineBlockFrequencyInfo) {
        size_t freq =
            machineBlockFrequencyInfo->getBlockFreq(&machineBasicBlock)
                .getFrequency();

        errs() << "   Basic block #" << machineBasicBlock.getNumber()
               << " freq=" << freq << "\n";
      }

      for (auto& machineInstruction : machineBasicBlock) {
        // TODO: look into frame index?
        if (machineInstruction.mayLoad()) numLoads++;
        if (machineInstruction.mayStore()) numStores++;

        // Checking operands for usage
        for (auto& machienOperand : machineInstruction.operands()) {
          if (!machienOperand.isReg() || !machienOperand.getReg()) continue;

          if (machienOperand.isUse()) reads++;
          if (machienOperand.isDef()) writes++;
        }
      }
    }

    errs() << "[RegAccessPostRA] Summary " << machineFunction.getName() << ": "
           << reads << " reads, " << writes << " writes, " << numLoads
           << " loads, " << numStores << " stores\n";

    return false;
  }
};

}  // end anonymous namespace

char RegisterAccessPostRAPass::ID = 0;

// static RegisterPass<RegisterAccessPostRAPass> X(
//  "reg-access-postra", "Count register accesses after register allocation",
//  false, false);
