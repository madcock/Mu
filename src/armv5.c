#include <stdint.h>
#include <stdbool.h>

#include "emulator.h"
#include "hardwareRegisters.h"
#include "memoryAccess.h"
#include "portability.h"
#include "armv5/CPU.h"
#include "m68k/m68k.h"
#include "debug/sandbox.h"


#define ARMV5_CYCLES_PER_OPCODE 2


bool armv5ServiceRequest;

static ArmCpu armv5Cpu;


static Boolean armv5MemoryAccess(ArmCpu* cpu, void* buf, UInt32 vaddr, UInt8 size, Boolean write, Boolean privileged, UInt8* fsr){
   //just gonna have ARM and 68k share the same bus for now, ARM may want fonts/bitmaps from ROM

   //Fake ARM 32 bit alignment convention
   //0x0XXXXXXX<->0x1XXXXXXX Normal memory access
   //0x20000000<->0x3XXXXXXX Mirrored range, realAddress = address - 0x20000000 + 0x00000002

   //this will still fail if ARM allocates a 16 bit aligned buffer and passes it to the m68k,
   //but it allows 16 bit aligned data to be executed from the 68k stack and works as a
   //temporary measure until I get MemChunkNew patched correctly

   //while executing wont support non 32 bit alignment regular accesses can for now
   /*
#if !defined(EMU_NO_SAFETY)
   //size is not a power of two or address isnt aligned to size
   if(size & (size - 1) || vaddr & (size - 1)){
      debugLog("ARM attempted unaligned access: address:0x%08X, size:%d\n", vaddr, size);
      return false;
   }
#endif
   */

   if(vaddr & 0x20000000)
      vaddr ^= 0x20000002;

   if(write){
      switch(size){
         case 1:
            m68k_write_memory_8(vaddr, *(uint8_t*)buf);
            return true;

         case 2:
            m68k_write_memory_16(vaddr, SWAP_16(*(uint16_t*)buf));
            return true;

         case 4:
            m68k_write_memory_32(vaddr, SWAP_32(*(uint32_t*)buf));
            return true;

         default:
            debugLog("Invalid ARMv5 write: address:0x%08X, size:%d\n", vaddr, size * 8);
            return false;
      }
   }
   else{
      switch(size){
         case 1:
            *(uint8_t*)buf = m68k_read_memory_8(vaddr);
            return true;

         case 2:
            *(uint16_t*)buf = SWAP_16(m68k_read_memory_16(vaddr));
            return true;

         case 4:
            *(uint32_t*)buf = SWAP_32(m68k_read_memory_32(vaddr));
            return true;

         default:
            debugLog("Invalid ARMv5 read: address:0x%08X, size:%d\n", vaddr, size * 8);
            return false;
      }
   }
}

static Boolean armv5Hypercall(ArmCpu* cpu){
   armv5ServiceRequest = true;
   return true;
}

static void armv5EmulErr(ArmCpu* cpu, const char* errStr){
   debugLog("%s\n", errStr);
}

static void armv5SetFaultAddr(struct ArmCpu* cpu, UInt32 adr, UInt8 faultStatus){
   //TODO
}

void armv5Reset(void){
   cpuInit(&armv5Cpu, 0x00000000/*PC, set by 68k while emulating*/, armv5MemoryAccess, armv5EmulErr, armv5Hypercall, armv5SetFaultAddr);
   armv5ServiceRequest = false;
}

uint32_t armv5StateSize(void){
   uint32_t size = 0;

   //armv5Cpu
   size += sizeof(uint32_t) * 16;//armv5Cpu.regs
   size += sizeof(uint32_t) * 2;//armv5Cpu.CPSR / armv5Cpu.SPSR
   size += sizeof(uint32_t) * 3 * 6;//armv5Cpu.bank_(usr/svc/abt/und/irq/fiq)
   size += sizeof(uint32_t) * 5;//armv5Cpu.extra_regs
   size += sizeof(uint16_t) * 3;//armv5Cpu.waiting(Irqs/Fiqs) / armv5Cpu.CPAR
   size += sizeof(uint32_t);//armv5Cpu.vectorBase

   //variables
   size += sizeof(uint8_t);//armv5ServiceRequest

   return size;
}

void armv5SaveState(uint8_t* data){
   uint32_t offset = 0;
   uint8_t index;

   //armv5Cpu
   for(index = 0; index < 16; index++){
      writeStateValue32(data + offset, armv5Cpu.regs[index]);
      offset += sizeof(uint32_t);
   }
   writeStateValue32(data + offset, armv5Cpu.CPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_usr.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_usr.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_usr.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_svc.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_svc.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_svc.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_abt.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_abt.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_abt.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_und.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_und.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_und.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_irq.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_irq.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_irq.SPSR);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_fiq.R13);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_fiq.R14);
   offset += sizeof(uint32_t);
   writeStateValue32(data + offset, armv5Cpu.bank_fiq.SPSR);
   offset += sizeof(uint32_t);
   for(index = 0; index < 5; index++){
      writeStateValue32(data + offset, armv5Cpu.extra_regs[index]);
      offset += sizeof(uint32_t);
   }
   writeStateValue16(data + offset, armv5Cpu.waitingIrqs);
   offset += sizeof(uint16_t);
   writeStateValue16(data + offset, armv5Cpu.waitingFiqs);
   offset += sizeof(uint16_t);
   writeStateValue16(data + offset, armv5Cpu.CPAR);
   offset += sizeof(uint16_t);
   writeStateValue32(data + offset, armv5Cpu.vectorBase);
   offset += sizeof(uint32_t);

   //variables
   writeStateValue8(data + offset, armv5ServiceRequest);
   offset += sizeof(uint8_t);
}

void armv5LoadState(uint8_t* data){
   uint32_t offset = 0;
   uint8_t index;

   //armv5Cpu
   for(index = 0; index < 16; index++){
      armv5Cpu.regs[index] = readStateValue32(data + offset);
      offset += sizeof(uint32_t);
   }
   armv5Cpu.CPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_usr.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_usr.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_usr.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_svc.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_svc.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_svc.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_abt.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_abt.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_abt.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_und.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_und.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_und.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_irq.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_irq.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_irq.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_fiq.R13 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_fiq.R14 = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   armv5Cpu.bank_fiq.SPSR = readStateValue32(data + offset);
   offset += sizeof(uint32_t);
   for(index = 0; index < 5; index++){
      armv5Cpu.extra_regs[index] = readStateValue32(data + offset);
      offset += sizeof(uint32_t);
   }
   armv5Cpu.waitingIrqs = readStateValue16(data + offset);
   offset += sizeof(uint16_t);
   armv5Cpu.waitingFiqs = readStateValue16(data + offset);
   offset += sizeof(uint16_t);
   armv5Cpu.CPAR = readStateValue16(data + offset);
   offset += sizeof(uint16_t);
   armv5Cpu.vectorBase = readStateValue32(data + offset);
   offset += sizeof(uint32_t);

   //variables
   armv5ServiceRequest = readStateValue8(data + offset);
   offset += sizeof(uint8_t);
}

uint32_t armv5GetRegister(uint8_t reg){
   return cpuGetRegExternal(&armv5Cpu, reg);
}

void armv5SetRegister(uint8_t reg, uint32_t value){
   cpuSetReg(&armv5Cpu, reg, value);
}

int32_t armv5Execute(int32_t cycles){
#if defined(EMU_DEBUG)
   uint32_t ranOpcodes = 0;
#endif

   armv5ServiceRequest = false;

#if defined(EMU_DEBUG) && defined(EMU_SANDBOX)
   sandboxSetCpuArch(SANDBOX_CPU_ARCH_ARMV5);
#endif

   //execution aborts on hypercall to request things from the m68k
   while(cycles > 0 && !armv5ServiceRequest){
      cpuCycleNoIrqs(&armv5Cpu);
      cycles -= ARMV5_CYCLES_PER_OPCODE;

#if defined(EMU_DEBUG)
      ranOpcodes++;
#endif
   }

#if defined(EMU_DEBUG)
   debugLog("%d ARM opcodes have been ran\n", ranOpcodes);
#endif

#if defined(EMU_DEBUG) && defined(EMU_SANDBOX)
   sandboxSetCpuArch(SANDBOX_CPU_ARCH_M68K);
#endif

   return cycles;
}

uint32_t armv5GetPc(void){
   return cpuGetRegExternal(&armv5Cpu, 15/*pc*/);
}
