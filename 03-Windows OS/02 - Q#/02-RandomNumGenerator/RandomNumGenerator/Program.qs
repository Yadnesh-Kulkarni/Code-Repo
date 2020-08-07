namespace RandomNumGenerator {

    open Microsoft.Quantum.Canon;
    open Microsoft.Quantum.Intrinsic;
    open Microsoft.Quantum.Measurement;
    open Microsoft.Quantum.Math;
    open Microsoft.Quantum.Convert;

    operation GenerateRandomBit() : Result{
        using (q = Qubit())
        {
            //Put Qubit in superposition
            H(q);

            //MResetZ measure the qubit and resets the qubit state to zero
            return MResetZ(q);
        }
    }

    operation SampleRandomNumberInRange(max : Int,min : Int) : Int {
        mutable output = 0;
        repeat{
            mutable bits = new Result[0];
            for(idxBit in 1..BitSizeI(max)){
                set bits += [GenerateRandomBit()];
            }
            set output = ResultArrayAsInt(bits);
        } until (output <= max and output >= min);
        return output;
    }

    @EntryPoint()
    operation SampleRandomNumber() : Int {
        let max = 50;
        let min = 20;
        Message($"Sampling a random number between 20 and {max}: ");
        return SampleRandomNumberInRange(max,min);
    }
}

