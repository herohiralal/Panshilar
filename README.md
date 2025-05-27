# Panshilar

The name means nothing, it just sounds good.

It's a C11 standard library for games with a heavy focus on minimalism, readability and cross-platform compatibility.

One of the stretch goals is to also support _multiple languages_ and provide a common base so that different libraries (in different languages) that use Panshilar, can interoperate a bit easier.

For now, even the idea of it, only revolves around:
- A common UTF-8 string type.
- Conversions to UTF-16 as required.
- Common internally-compatible allocators.
- Common slice and array types.

Some constraints/styling:
- No conditional compilation allowed in _most_ header files.
  - This maintains a clear, consistent and reliable API across different environments and platforms.
  - This would also help with reflection-based code generation for different languages.
- C11 only.
  - C++ ABI is unstable and it's easier to port.
- No exceptions, no crashes. Return errors as values.
- Avoid delegate passing.
  - While a lot of languages are compatible with C calling convention, it's not the default.
  - For languages like Jai/Odin, that use an implicit context system, this is even more annoying.
- No storing delegates (and by extension 'allocators') in structs or global variables.
  - Pain when it comes to hot-reloading.
- Avoid globals, but not outright banned, if they're localised properly.
- Avoid UCRT dependencies.
  - This isn't a hard rule, because functionality comes first, but avoid as much as possible.
