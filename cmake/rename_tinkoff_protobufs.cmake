
file(READ "${TARGET_FILE}" TEXT)
string(REPLACE "namespace public {" "namespace public_ {" TEXT "${TEXT}")
string(REPLACE "tinkoff::public::" "tinkoff::public_::" TEXT "${TEXT}")
file(WRITE "${TARGET_FILE}" "${TEXT}")