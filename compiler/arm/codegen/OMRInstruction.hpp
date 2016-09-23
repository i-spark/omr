/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

#ifndef OMR_ARM_INSTRUCTIONBASE_INCL
#define OMR_ARM_INSTRUCTIONBASE_INCL

/*
 * The following #define and typedef must appear before any #includes in this file
 */
#ifndef OMR_INSTRUCTION_CONNECTOR
#define OMR_INSTRUCTION_CONNECTOR
   namespace OMR { namespace ARM { class Instruction; } }
   namespace OMR { typedef OMR::ARM::Instruction InstructionConnector; }
#else
   #error OMR::ARM::Instruction expected to be a primary connector, but a OMR connector is already defined
#endif

#include "compiler/codegen/OMRInstruction.hpp"

#include "arm/codegen/ARMOps.hpp"
#include "codegen/InstOpCode.hpp"
#include "codegen/GCStackMap.hpp"
#include "codegen/RegisterConstants.hpp"

class TR_ARMConditionalBranchInstruction;
class TR_ARMDepImmInstruction;
class TR_ARMImmInstruction;
namespace TR { class CodeGenerator; }
namespace TR { class MemoryReference; }
namespace TR { class RegisterDependencyConditions; }
struct TR_RegisterPressureState;

namespace OMR
{

namespace ARM
{

class OMR_EXTENSIBLE Instruction : public OMR::Instruction
   {

   public:

   Instruction(TR::CodeGenerator *cg, TR::InstOpCode::Mnemonic op, TR::Node *node = 0);
   Instruction(TR::CodeGenerator *cg, TR::Instruction *precedingInstruction, TR::InstOpCode::Mnemonic op, TR::Node *node = 0);

   virtual char *description() { return "ARM"; }

   virtual Kind getKind() { return IsNotExtended; }

   TR_ARMOpCode& getOpCode()                       {return _opcode;}
   TR_ARMOpCodes getOpCodeValue()                  {return _opcode.getOpCodeValue();}
   TR_ARMOpCodes getRecordFormOpCode()             {return _opcode.getRecordFormOpCodeValue();}
   TR_ARMOpCodes setOpCodeValue(TR_ARMOpCodes op)  {return _opcode.setOpCodeValue(op);}

   void remove();

   int32_t  getBlockIndex()            { return _blockIndex; }
   void     setBlockIndex(int32_t i)   { _blockIndex = i; }

   void ARMNeedsGCMap(uint32_t mask);

   TR_ARMConditionCode getConditionCode()                          {return _conditionCode;}
   TR_ARMConditionCode setConditionCode(TR_ARMConditionCode conds) {return (_conditionCode = conds);}

   virtual TR::Snippet *getSnippetForGC() {return NULL;}

   virtual TR::Register *getMemoryDataRegister();

   virtual uint8_t *generateBinaryEncoding();

   void generateConditionBinaryEncoding(uint8_t *instructionStart);

   virtual int32_t estimateBinaryLength(int32_t currentEstimate);

   virtual bool setWriteBack()   {return false;}
   virtual bool isWriteBack()    {return false;}

   virtual bool isLabel()        {return _opcode.getOpCodeValue() == ARMOp_label;}

   virtual TR::RegisterDependencyConditions *getDependencyConditions()
      {
      return _conditions;
      }
   TR::RegisterDependencyConditions *setDependencyConditions(TR::RegisterDependencyConditions *cond)
      {
      return (_conditions = cond);
      }

   virtual void assignRegisters(TR_RegisterKinds kindToBeAssigned);

   virtual bool refsRegister(TR::Register *reg);
   virtual bool defsRegister(TR::Register *reg);
   virtual bool usesRegister(TR::Register *reg);

   virtual bool defsRealRegister(TR::Register *reg);

   virtual bool dependencyRefsRegister(TR::Register *reg);

   virtual TR_ARMConditionalBranchInstruction *getARMConditionalBranchInstruction();

   virtual TR::Register *getPrimaryTargetRegister() {return NULL;}

// The following safe virtual downcast method is used under debug only
// for assertion checking
#if defined(DEBUG) || defined(PROD_WITH_ASSUMES)
   virtual TR_ARMImmInstruction *getARMImmInstruction();
#endif


   virtual TR::Register *getMemoryBase()                     {return NULL;}
   virtual TR::Register *getMemoryIndex()                    {return NULL;}
   virtual int32_t      getOffset()                         {return 0;}
   // @@ virtual bool  usesCountRegister()                        {return _opcode.usesCountRegister();}
   /* @@
   virtual bool  setsCountRegister()
           {
           if (_opcode.getOpCodeValue() == TR::InstOpCode::beql)
              return true;
           return isCall()|_opcode.setsCountRegister();
           }
   */
   bool     isRecordForm()          {return _opcode.isRecordForm();}
   bool     hasRecordForm()         {return _opcode.hasRecordForm();}
   bool     singleFPOp()            {return _opcode.singleFPOp();}
   bool     doubleFPOp()            {return _opcode.doubleFPOp();}
   bool     gprOp()                 {return _opcode.gprOp();}
   bool     fprOp()                 {return _opcode.fprOp();}
   // @@ bool     useAlternateFormat()    {return _opcode.useAlternateFormat();}
   // @@ bool     useAlternateFormatx()   {return _opcode.useAlternateFormatx();}
   bool     readsCarryFlag()        {return _opcode.readsCarryFlag();}
   bool     setsCarryFlag()         {return _opcode.setsCarryFlag();}
   bool     setsOverflowFlag()      {return _opcode.setsOverflowFlag();}
   // @@ bool     isUpdate()              {return _opcode.isUpdate();}
   // @@ bool     isTrap()                {return _opcode.isTrap();}
   // @@ bool     isTMAbort()             {return _opcode.isTMAbort();}
   // @@ bool     isRegCopy()             {return _opcode.isRegCopy();}
   // @@ bool     isDoubleWord()          {return _opcode.isDoubleWord();}
   // @@ bool     isRotateOrShift()       {return _opcode.isRotateOrShift();}
   // @@ bool     isCompare()             {return _opcode.isCompare();}
   // @@ bool     isLongRunningFPOp()     {return _opcode.isLongRunningFPOp();}
   // @@ bool     isFXMult()              {return _opcode.isFXMult();}
   // @@ bool      isBreakPoint() {return (_index & BreakPoint) != 0;}
   // @@ void      setBreakPoint(bool v) {v ? _index |= BreakPoint : _index &= ~BreakPoint;}

   // @@ virtual bool     isLoad()           {return _opcode.isLoad();}
   // @@ virtual bool     isStore()          {return _opcode.isStore();}
   virtual bool     isBranchOp()       {return _opcode.isBranchOp();}
   virtual bool     isExceptBranchOp() {return false; }
   // @@ virtual bool     isSync()           {return _opcode.isSync();}
   // @@ virtual bool     isAdmin()          {return _opcode.isAdmin();}
   // @@ virtual bool     is4ByteLoad()      {return (getOpCodeValue() == TR::InstOpCode::lwz);}
   virtual bool     isDebugFence()     {return false;};
   virtual int32_t  getMachineOpCode();
   // @@ virtual bool     isBeginBlock();
   // @@ virtual bool     isFloat()          {return _opcode.isFloat();}
   // @@ virtual bool     isVMX()            {return _opcode.isVMX();}
   // @@ virtual bool     isVSX()            {return _opcode.isVSX();}
   // @@ virtual bool     isSyncSideEffectFree()  {return _opcode.isSyncSideEffectFree();}
   // @@ virtual bool     isCall();

   /*
    * Maps to TIndex in Instruction. Here we set values specific to PPC CodeGen.
    *
    * A 32-bit field where the lower 24-bits contain an integer that represents an
    * approximate ordering of instructions.
    *
    * The upper 8 bits are used for flags.
    * Instruction flags encoded by their bit position.  Subclasses may use any
    * available bits between LastBaseFlag and MaxBaseFlag inclusive.
    */
   enum
      {
      WillBePatched        = 0x08000000
      };

   bool      willBePatched() {return (_index & WillBePatched) != 0; }
   void      setWillBePatched(bool v = true) { v? _index |= WillBePatched : _index &= ~WillBePatched; }



   private:
      TR_ARMOpCode   _opcode;
      int32_t       _blockIndex;
      TR_ARMConditionCode                  _conditionCode;
      TR::RegisterDependencyConditions *_conditions;
      bool        _asyncBranch;


   };

}

}

#endif