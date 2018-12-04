LLVM_CXXCONFIGS="$(llvm-config --cxxflags)"
LLVMLDFLAGS="$(llvm-config --ldflags --libs --system-libs)"

g++ \
    $LLVM_CXXCONFIGS \
    $LLVMLDFLAGS \
    -o test1.out test1.cpp \
    -lclangTooling \
    -lclangFrontendTool \
    -lclangFrontend \
    -lclangDriver \
    -lclangSerialization \
    -lclangCodeGen \
    -lclangParse \
    -lclangSema \
    -lclangASTMatchers \
    -lclangStaticAnalyzerFrontend \
    -lclangStaticAnalyzerCheckers \
    -lclangStaticAnalyzerCore \
    -lclangAnalysis \
    -lclangARCMigrate \
    -lclangRewriteFrontend \
    -lclangRewrite \
    -lclangEdit \
    -lclangAST \
    -lclangLex \
    -lclangBasic 
    
