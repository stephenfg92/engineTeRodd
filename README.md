# Lua

Para usar o lua no projeto, basta:
  - Incluir o header sol.hpp
  - Descomentar a instrução LDFLAGS += -L"X:/pasta/do/projeto/ssge/lib" no makefile
  - Descomentar a instrução LDLIBS += -llua54 no makefile
