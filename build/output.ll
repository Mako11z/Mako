; ModuleID = 'main'
source_filename = "main"

define i32 @main() {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  store i32 1, ptr %i, align 4
  store i32 11, ptr %i, align 4
  ret i32 0
}
