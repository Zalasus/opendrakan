
#ifndef INCLUDE_UNITS_H_
#define INCLUDE_UNITS_H_

namespace od
{

    class Units
    {
    public:

        static constexpr int WU_PER_LU = (1 << 11);

        template <typename T>
        static T lenthUnitsToWorldUnits(T lu)
        {
            return lu * T(WU_PER_LU);
        }

        template <typename T>
        static T worldUnitsToLengthUnits(T wu)
        {
            return wu * T(1.0/WU_PER_LU);
        }

    };

}

#endif
