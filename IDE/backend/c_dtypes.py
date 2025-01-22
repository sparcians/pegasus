class c_style_int:
    def __init__(self, value):
        """Initialize the integer value. This method should be overridden in derived classes for sign handling."""
        self.value = value

    def __repr__(self):
        return f"{self.__class__.__name__}({self.value})"

    # Arithmetic operations
    def __add__(self, other):
        return self.__class__(self.value + other.value)

    def __sub__(self, other):
        return self.__class__(self.value - other.value)

    def __mul__(self, other):
        return self.__class__(self.value * other.value)

    def __floordiv__(self, other):
        return self.__class__(self.value // other.value)

    def __mod__(self, other):
        return self.__class__(self.value % other.value)

    # Bitwise operations
    def __and__(self, other):
        return self.__class__(self.value & other.value)

    def __or__(self, other):
        return self.__class__(self.value | other.value)

    def __xor__(self, other):
        return self.__class__(self.value ^ other.value)

    def __lshift__(self, other):
        return self.__class__(self.value << other.value)

    def __rshift__(self, other):
        return self.__class__(self.value >> other.value)

    # Comparison operations
    def __eq__(self, other):
        return self.value == other.value

    def __lt__(self, other):
        return self.value < other.value

    def __le__(self, other):
        return self.value <= other.value

    def __gt__(self, other):
        return self.value > other.value

    def __ge__(self, other):
        return self.value >= other.value

    @staticmethod
    def sext32(x):
        """Sign-extend a 32-bit signed integer to 64 bits."""
        if x & 0x80000000:  # If negative
            return type(x)(x - 0x100000000)
        return type(x)(x)

    @staticmethod
    def sext(x, pos):
        """Sign-extend an integer `x` with a specified bit position."""
        if x & (1 << (pos - 1)):
            return type(x)(x - (1 << pos))
        return type(x)(x)

    @staticmethod
    def sext_xlen(x, xlen):
        """Sign-extend with a bit-length given by xlen."""
        return type(x).sext(x, xlen)

    # Static extension methods
    @staticmethod
    def zext32(x):
        """Zero-extend a 32-bit unsigned integer to 64 bits."""
        return type(x)(x & 0xFFFFFFFF)

    @staticmethod
    def zext(x, pos):
        """Zero-extend an integer `x` with a specified bit position."""
        return type(x)(x & ((1 << pos) - 1))

    @staticmethod
    def zext_xlen(x, xlen):
        """Zero-extend with a bit-length given by xlen."""
        return type(x).zext(x, xlen)

class reg_t(c_style_int):
    def __init__(self, value):
        # Ensure value is unsigned 64-bit
        super().__init__(value & 0xFFFFFFFFFFFFFFFF)

class sreg_t(c_style_int):
    def __init__(self, value):
        # Ensure value is signed 64-bit
        super().__init__(value & 0xFFFFFFFFFFFFFFFF)
        if self.value >= 0x8000000000000000:  # If negative, extend sign
            self.value -= 0x10000000000000000

def convert_to_hex(value):
    """Convert a value to a hex string."""
    if isinstance(value, c_style_int):
        return f"0x{value.value:016x}"

    return f"0x{value:016x}"
