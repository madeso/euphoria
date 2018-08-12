#ifndef EUPHORIA_DUK_CONFIG_H
#define EUPHORIA_DUK_CONFIG_H

// todo: add this as a option during build
#if 1
// provide c++ class name class when its not added to the registry
#define CLASS_ARG(x) , x
#define CLASS_NAME(x) x
#else
#define CLASS_ARG(x)
#define CLASS_NAME(x)
#endif

#endif  // EUPHORIA_DUK_CONFIG_H
