using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using Microsoft.Xna.Framework;


namespace Nebula
{
    namespace Game
    {
        /*
         * Entity
         */
        [StructLayout(LayoutKind.Sequential)]
        public class Entity
        {
            private readonly uint id = uint.MaxValue;

            public Entity(uint id)
            {
                this.id = id;
            }

            public uint Id
            {
                get
                {
                    return id;
                }
            }

            public override string ToString() { return this.id.ToString(); }

            /// <summary>
            /// Check whether this entity is valid (alive)
            /// </summary>
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public static extern bool IsValid();

            /// <summary>
            /// Retrieve the transform of an entity if it is registered to the component
            /// </summary>
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            public extern Matrix GetTransform(Game.Entity entity);
        }

        public struct InstanceId
        {
            private uint id;

            public InstanceId(uint id)
            {
                this.id = id;
            }

            public uint Id
            {
                get
                {
                    return id;
                }
            }
        }
    }

    public class EntityManager
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Game.Entity CreateEntity();
    }

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

