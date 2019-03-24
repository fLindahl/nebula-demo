using System;
using System.Runtime.InteropServices;

namespace Nebula
{

public class Debug
{
    [DllImport ("__Internal", EntryPoint="N_Print")]
    public static extern void Log(string val);
}

public class Entity
{
    [DllImport ("__Internal", EntryPoint="Scripting::Api::GetTransform")]
}

// [DllImport ("__Internal", EntryPoint="Foobar", CharSet=CharSet.Ansi)]
// static extern void Foobar(
//     [MarshalAs (UnmanagedType.CustomMarshaler,
//         MarshalTypeRef=typeof(StringMarshaler))]
//     String val
// );

}

