{
  lib,
  runCommand,
  clang-tools,
  cmake,
  cmake-format,
}:
{
  src ? ../.,
}:

let
  sourceFiles = lib.fileset.toSource {
    root = src;
    fileset = lib.fileset.unions [
      (src + /launcher)
      (src + /tests)
      (src + /buildconfig)
    ];
  };
in
runCommand "projt-code-check"
  {
    nativeBuildInputs = [
      clang-tools
      cmake
      cmake-format
    ];
  }
  ''
    echo "Running ProjT Launcher code quality checks..."

    echo "Checking C++ code formatting..."
    find ${sourceFiles} -type f \( -name "*.cpp" -o -name "*.h" \) | while read file; do
      if ! clang-format --dry-run --Werror "$file" 2>/dev/null; then
        echo "Format error in: $file"
      fi
    done

    echo "Checking for common code issues..."

    todoCount=$(grep -r "TODO\\|FIXME" ${sourceFiles} --include="*.cpp" --include="*.h" 2>/dev/null | wc -l)
    echo "Found $todoCount TODO/FIXME comments"

    if grep -r "qDebug\\|std::cout" ${sourceFiles} --include="*.cpp" 2>/dev/null | grep -v "// DEBUG" > /dev/null; then
      echo "Warning: Debug statements found in code"
    fi

    echo "Code quality check completed!"
    touch $out
  ''
