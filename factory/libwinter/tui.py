import sys
import math
import atexit
import shutil


class Escape:
    CSI = "\u001b["
    ERASE_LINE = f"{CSI}2K\r"
    MOVE_UP = f"{CSI}1A"
    CURSOR_PREVIOUS_LINE = f"{CSI}1F"
    CURSOR_PREVIOUS_LINE_NUM = f"{CSI}{{count}}F"
    ERASE_AFTER_CURSOR = f"{CSI}0J"
    RESET = f"{CSI}0m"
    COLOR24 = f"{CSI}38;2;{{r}};{{g}};{{b}}m"
    BOLD = f"{CSI}1m"
    FAINT = f"{CSI}2m"
    ITALIC = f"{CSI}3m"
    UNDERLINE = f"{CSI}4m"
    INVERT = f"{CSI}7m"
    STRIKETHROUGH = f"{CSI}9m"
    NORMAL = f"{CSI}22m"
    OVERLINED = f"{CSI}53m"


def gradient(a, b, v):
    v = max(0.0, min(v, 1.0))
    r = a[0] + v * (b[0] - a[0])
    g = a[1] + v * (b[1] - a[1])
    b = a[2] + v * (b[2] - a[2])
    return r, g, b


class Colors:
    reset = Escape.RESET

    @staticmethod
    def rgb(r, g=None, b=None):
        if isinstance(r, tuple):
            r, g, b = r

        if r > 1 or g > 1 or b > 1:
            r, g, b = r / 255, g / 255, b / 255

        r, g, b = [int(x * 255) for x in (r, g, b)]
        return Escape.COLOR24.format(r=r, g=g, b=b)


class Updateable:
    def __init__(self):
        self._line_count = 0
        self._clear_on_next = False

    def write(self, text):
        if self._clear_on_next:
            self.clear()
            self._clear_on_next = False
        self._line_count += text.count("\n")
        sys.stdout.write(text)

    def reset(self):
        self._line_count = 0

    def clear(self):
        if self._line_count > 0:
            clear_lines = Escape.CURSOR_PREVIOUS_LINE_NUM.format(count=self._line_count)
            clear_lines += Escape.ERASE_AFTER_CURSOR
            sys.stdout.write(clear_lines)
            sys.stdout.flush()
            self._line_count = 0

    def __enter__(self):
        self._clear_on_next = True

    def __exit__(self, exc_type, exc_value, exc_traceback):
        sys.stdout.flush()

    def flush(self):
        sys.stdout.flush()


class Segment:
    def __init__(self, width, color=(1.0, 1.0, 1.0), char="▓"):
        self.width = width
        self.color = color
        self.char = char


class Bar:
    FILL_CHAR = "░"
    FILL_COLOR = (0.4, 0.4, 0.4)

    def __init__(self, width=50, fill=True):
        self.width = width
        self.fill = fill

    def draw(self, output, *segments, end="\n"):
        segments = list(segments)

        for n, segment in enumerate(segments):
            if isinstance(segment, tuple):
                segments[n] = Segment(*segment)

        # Add end segment if needed.
        if self.fill:
            left_to_fill = 1.0 - sum(s.width for s in segments)
            segments.append(
                Segment(left_to_fill, color=self.FILL_COLOR, char=self.FILL_CHAR)
            )

        # Largest remainder method allocation
        seg_lengths = [math.floor(s.width * self.width) for s in segments]
        seg_fract = [(n, (s.width * self.width) % 1.0) for n, s in enumerate(segments)]
        seg_fract.sort(key=lambda x: x[1], reverse=True)
        remainder = self.width - sum(seg_lengths)

        for n in range(remainder):
            seg_lengths[seg_fract[n][0]] += 1

        # Now draw
        buf = ""
        for n, seg in enumerate(segments):
            buf += Colors.rgb(*seg.color) + (seg.char * seg_lengths[n])
        buf += Colors.reset + end

        output.write(buf)


class Columns:
    def __init__(self, *columns):
        self._columns = columns

    def draw(self, output, *values):
        n = 0
        for v in values:
            if isinstance(v, str) and v.startswith(Escape.CSI):
                output.write(v)
                continue
            if isinstance(v, tuple) and len(v) == 3:
                output.write(Colors.rgb(v))
                continue

            c = self._columns[n]
            formatter = f"{{: {c}}}"
            output.write(formatter.format(v))

            n += 1

        output.write(Colors.reset + "\n")

    def __len__(self):
        return sum(int(s[1:]) for s in self._columns)


def width():
    columns, lines = shutil.get_terminal_size()
    return columns


def reset_terminal():
    print(Escape.RESET, end="")
    sys.stdout.flush()


atexit.register(reset_terminal)


# Helpful aliases

rgb = Colors.rgb
reset = Escape.RESET
bold = Escape.BOLD
faint = Escape.FAINT
italic = Escape.ITALIC
underline = Escape.UNDERLINE
invert = Escape.INVERT
strikethrough = Escape.STRIKETHROUGH
normal = Escape.NORMAL
overlined = Escape.OVERLINED
