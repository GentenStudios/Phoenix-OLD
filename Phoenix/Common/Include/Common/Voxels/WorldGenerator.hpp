class WorldGenerator {

    public:
        struct Params
        {
            float strength;
            float size;
            int octaves;
            float persistence;
            float height;
        };

    private:
        static Params params;

    public:
        static void setParams(Params paramsIn);
        static Params getParams();
};