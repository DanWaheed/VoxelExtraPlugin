export void VoxelBoxBlur(
    uniform int32 ArraySize,
    const uniform uint16 InputArray[],
    uniform int32 Amount,
    uniform int32 SmoothedArray[])
{
    foreach (index = 0 ... ArraySize)
    {
        if (InputArray[index] == 0) 
        {
            SmoothedArray[index] = 0;
            continue;
        }

        int32 sum = InputArray[index];
        int32 count = 1;

        // Box blur with skip logic
        for (int i = 1; i <= Amount; ++i)
        {
            // Skip elements with a value of 0
            if (index - i >= 0 && InputArray[index - i] != 0)
            {
                sum += InputArray[index - i];
                ++count;
            }

            if (index + i < ArraySize && InputArray[index + i] != 0)
            {
                sum += InputArray[index + i];
                ++count;
            }

        }

        SmoothedArray[index] = sum / count;
    }
}