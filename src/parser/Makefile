LLVM_CXXCONFIGS= $(shell llvm-config --cxxflags)
LLVMLDFLAGS= $(shell llvm-config --ldflags --libs --system-libs)

OBJS = main.cpp config.o generator.o result.o

.PHONY: test
test: $(OBJS)
	g++ \
		$(LLVM_CXXCONFIGS) \
		$(LLVMLDFLAGS) \
		-o main.out \
		$(OBJS) \
		-fexceptions \
		-lyaml-cpp \
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
	

.PHONY: all
all: test1.cpp config.o
	g++ \
		$(LLVM_CXXCONFIGS) \
		$(LLVMLDFLAGS) \
		-o test1.out \
		test1.cpp config.o \
		-fexceptions \
		-lyaml-cpp \
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

config.o: config.h config.cpp
	g++ -c config.cpp

generator.o: generator.h generator.cpp utils.h
	g++ -c generator.cpp

result.o: result.h result.cpp
	g++ -c result.cpp

clean:
	rm -rf *.out *.o
