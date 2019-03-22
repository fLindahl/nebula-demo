using System;
using System.Runtime.InteropServices;

namespace Util
{
    internal unsafe struct String
    {
        public string heap;
        public fixed char local[20];
        public int strLen;
        public int heapBufferSize;
    }
}

public class Nebula
{
    [DllImport ("__Internal", EntryPoint="N_Print")]
    static extern void Print(string val);


    [DllImport ("__Internal", EntryPoint="Foobar", CharSet=CharSet.Ansi)]
    static extern void Foobar(
        [MarshalAs (UnmanagedType.CustomMarshaler,
            MarshalTypeRef=typeof(StringMarshaler))]
        String val
    );

    class StringMarshaler : ICustomMarshaler {

		private static StringMarshaler Instance = new StringMarshaler ();

		public static ICustomMarshaler GetInstance (string s)
		{
			return Instance;
		}

		public void CleanUpManagedData (object o)
		{
		}

		public void CleanUpNativeData (IntPtr pNativeData)
		{
			Print($"# StringMarshaler.CleanUpManagedData ({pNativeData:x})\n");
            Marshal.FreeHGlobal(pNativeData);
		}

		public int GetNativeDataSize ()
		{
			return IntPtr.Size;
		}

		public IntPtr MarshalManagedToNative (object obj)
		{
			string s = obj as string;
			if (s == null)
				return IntPtr.Zero;
                
			Util.String str;
            str.heap = s;
            str.strLen = s.Length;
            str.heapBufferSize = s.Length * 1; // 8 bit characters

            IntPtr ret = Marshal.AllocHGlobal(System.Runtime.InteropServices.Marshal.SizeOf(typeof(Util.String)));
            Marshal.StructureToPtr(str, ret, false);

			Print($"# StringMarshaler.MarshalNativeToManaged for `{s}'={ret:x}\n");
			return ret;
		}

		public object MarshalNativeToManaged (IntPtr pNativeData)
		{
			string s = Marshal.PtrToStringAnsi(pNativeData);
			Print($"# StringMarshaler.MarshalNativeToManaged ({pNativeData:x})=`{s}'\n");
			return s;
		}
	}

    static public void Main ()
    {
        Print("Hello Mono World");
        Foobar("Foobar >> Util::String\n");
    }
}