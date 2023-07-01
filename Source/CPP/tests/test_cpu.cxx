#include "../CPU.hxx"

/**
 * @brief Test the CPU class
 * 
 * @note This test assumes that the register and store line tests have passed.
 * 
 * @return true if the tests pass.
 * @return false if any test fils.
 */
bool TestCpu()
{
    StoreLines storeLines = StoreLines();

    storeLines[1].SetValue(0b0100000000001010);     // LDN 10
    storeLines[2].SetValue(0b1000000000001011);     // SUB 11
    storeLines[3].SetValue(0b0110000000001100);     // STO 12
    storeLines[4].SetValue(0b1100000000000000);     // CMP (SKN)
    storeLines[5].SetValue(0b0000000000001100);     // JMP 12 (should be store line 1 to make the next instruction executed store line 2)
    storeLines[6].SetValue(0b0010000000001011);     // JRP 11 (Add 9 to CI, currently 6)
    storeLines[16].SetValue(0b1110000000000000);    // HALT
    storeLines[10].SetValue(0xfffffff6);            // Number 10 (negated so that load will get the positive number).
    storeLines[11].SetValue(9);                     // Number 9
    storeLines[12].SetValue(0);                     // Number 0

    Cpu cpu = Cpu(storeLines);
    cpu.Reset();                                    // We should now be ready to run the application.

    if (cpu.IsStopped())
    {
        printf("CPU is stopped after reset.\n");
        return(false);
    }
    if (cpu.PI().GetValue() != 0)
    {
        printf("PI is not zero after reset.\n");
        return(false);
    }
    if (cpu.CI().GetValue() != 0)
    {
        printf("CI is not zero after reset.\n");
        return(false);
    }
    if (cpu.Accumulator().GetValue() != 0)
    {
        printf("Accumulator is not zero after reset.\n");
        return(false);
    }
    //
    //  LDN 10
    //
    if (!cpu.SingleStep())
    {
        printf("SingleStep returned false.\n");
        return(false);
    }
    if (cpu.CI().GetValue() != 1)
    {
        printf("CI is not 1 after LDN 10.\n");
        return(false);
    }
    if (cpu.Accumulator().GetValue() != 10)
    {
        printf("Accumulator is %ld when if should be 10 after LDN 10.\n", (long) cpu.Accumulator().GetValue());
        return(false);
    }
    //
    //  SUB 11
    //
    cpu.SingleStep();
    if (cpu.CI().GetValue() != 2)
    {
        printf("CI is not 2 after SUB 11.\n");
        return(false);
    }
    if (cpu.Accumulator().GetValue() != 1)
    {
        printf("Accumulator is not 1 after SUB 11.\n");
        return(false);
    }
    //
    //  STO 12
    //
    cpu.SingleStep();
    if (cpu.Accumulator().GetValue() != 1)
    {
        printf("Accumulator is not 1 after STO 12.\n");
        return(false);
    }
    if (storeLines[12].GetValue() != 1)
    {
        printf("Store line 12 is not 1 after STO 12.\n");
        return(false);
    }
    //
    //  CMP (SKN)
    //
    cpu.SingleStep();
    if (cpu.CI().GetValue() != 4)
    {
        printf("CI is not 4 after CMP (SKN).\n");
        return(false);
    }
    //
    //  Execute some repeats of the above.
    //
    cpu.SingleStep();   // JMP 12
    cpu.SingleStep();   // SUB 11
    cpu.SingleStep();   // STO 12
    cpu.SingleStep();   // CMP (SKN)
    //
    //  JRP 11
    //
    cpu.SingleStep();
    if (cpu.CI().GetValue() != 15)
    {
        printf("CI is not 15 after JRP 11.\n");
        return(false);
    }
    //
    //  HALT
    //
    cpu.SingleStep();
    if (!cpu.IsStopped())
    {
        printf("CPU is not stopped after HALT.\n");
        return(false);
    }
    if (cpu.SingleStep())
    {
        printf("CPU is not stopped after HALT.\n");
        return(false);
    }
    //
    //  Check that store line 12 has been updated.
    //
    if (storeLines[12].GetValue() != ((int32_t) 0xfffffff8))
    {
        printf("Store line 12 is not 0xfffffff8 after HALT.\n");
        return(false);
    }

    return(true);
}