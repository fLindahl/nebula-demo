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
        public struct Entity : IEquatable<Entity>
        {
            private UInt32 id;
            
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

            /// <summary>
            /// This entitys transform
            /// </summary>
            public Matrix Transform
            {
                get
                {
                    return GetTransform(this);
                }

                set
                {
                    // TODO: Send set transform message.
                    // maybe check if this entity is registered first and register it if necessary?
                    Console.WriteLine(value);
                    SetTransform(this, value);
                }
            }

            public bool IsValid()
            {
                return IsAlive(this);
            }


            public override string ToString() { return this.id.ToString(); }

            /// <summary>
            /// Check whether this entity is valid (alive)
            /// </summary>
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            private static extern bool IsAlive(Entity e);

            /// <summary>
            /// Retrieve the transform of an entity if it is registered to the component
            /// </summary>
            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            private extern Matrix GetTransform(Game.Entity entity);

            [MethodImplAttribute(MethodImplOptions.InternalCall)]
            private extern void SetTransform(Game.Entity entity, ref Matrix mat);

            public bool Equals(Entity other)
            {
                return this.id == other.id;
            }
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

