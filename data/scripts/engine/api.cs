using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Microsoft.Xna.Framework;

class Game
{
    [MethodImplAttribute(MethodImplOptions.InternalCall)]
    public extern static Matrix GetTransform(uint entity);
}

namespace Nebula
{


public class Debug
{
    [DllImport ("__Internal", EntryPoint="N_Print")]
    public static extern void Log(string val);
}

// public class Entity
// {
//     [DllImport ("__Internal", EntryPoint="Scripting::Api::GetTransform")]
// }

// [DllImport ("__Internal", EntryPoint="Foobar", CharSet=CharSet.Ansi)]
// static extern void Foobar(
//     [MarshalAs (UnmanagedType.CustomMarshaler,
//         MarshalTypeRef=typeof(StringMarshaler))]
//     String val
// );

}

