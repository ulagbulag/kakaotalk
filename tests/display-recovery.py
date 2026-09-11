#!/usr/bin/env python3
# SPDX-License-Identifier: Unlicense
"""Exercise the launcher without touching a real Wine session or installation."""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


LAUNCHER = Path(__file__).resolve().parents[1] / "kakaotalk"
MOCK = r'''#!/usr/bin/env python3
import json
import os
from pathlib import Path
import sys

name = Path(sys.argv[0]).name
args = sys.argv[1:]
prefix = os.environ.get("WINEPREFIX", "")
with open(os.environ["MOCK_LOG"], "a") as log:
    log.write(json.dumps([name, args, prefix]) + "\n")
if name == "timeout":
    assert args[:2] == ["--foreground", "30s"], args
    if os.environ.get("MOCK_FAIL") == "timeout:" + args[2]:
        sys.exit(124)
    os.execvp(args[2], args[2:])
if name in ("wineboot", "wineserver", "wine"):
    assert prefix == os.environ["MOCK_PREFIX"], prefix
    if os.environ.get("MOCK_FAIL") == name:
        sys.exit(42)
    if name == "wine":
        assert args == [prefix + "/drive_c/Program Files (x86)/Kakao/KakaoTalk/KakaoTalk.exe"]
    sys.exit(0)
if name == "ls":
    assert args == [os.environ["MOCK_PREFIX"] + "/drive_c/Program Files (x86)/Kakao/KakaoTalk/KakaoTalk.exe"]
    sys.exit(int(os.environ.get("MOCK_MISSING", "0")))
if name == "cat":
    if not args:
        print(sys.stdin.read(), end="")
    else:
        assert args == [os.environ["MOCK_PREFIX"] + "/winetricks.log"]
        print("cjkfonts")
    sys.exit(0)
raise AssertionError("Unexpected installer command: " + name)
'''


class DisplayRecoveryTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="kakaotalk-recovery-test-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.log = self.root / "calls.jsonl"
        shim = self.root / "mock"
        shim.write_text(MOCK)
        shim.chmod(0o755)
        for name in ("wine", "wineboot", "wineserver", "timeout", "ls", "cat", "curl", "winetricks"):
            (self.root / name).symlink_to(shim)
        self.prefix = str(Path.home() / ".local/share/kakaotalk")
        self.env = dict(os.environ, PATH=f"{self.root}:/usr/bin:/bin",
                        MOCK_LOG=str(self.log), MOCK_PREFIX=self.prefix,
                        WINEPREFIX="/unrelated-wine-prefix", XMODIFIERS="@im=test")
        self.env.pop("BASH_ENV", None)
        self.env.pop("LD_PRELOAD", None)

    def launch(self, *args, **env):
        self.log.unlink(missing_ok=True)
        result = subprocess.run(["/bin/bash", str(LAUNCHER), *args],
                                env=dict(self.env, **env), text=True,
                                capture_output=True, timeout=10)
        self.calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        self.actions = [(name, args) for name, args, _ in self.calls
                        if name in ("wineboot", "wineserver", "wine")]
        return result

    def test_normal_launch_does_not_restart(self):
        result = self.launch()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual([name for name, _ in self.actions], ["wine"])
        self.assertNotIn("timeout", [name for name, _, _ in self.calls])

    def test_help_does_not_access_wine(self):
        for option in ("--help", "-h"):
            with self.subTest(option=option):
                result = self.launch(option)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn("--recover-display", result.stdout)
                self.assertEqual([name for name, _, _ in self.calls], ["cat"])

    def test_recovery_waits_for_its_own_prefix_before_launch(self):
        result = self.launch("--recover-display")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(self.actions[:2], [("wineboot", ["--end-session", "--shutdown"]),
                                          ("wineserver", ["--wait"])])
        self.assertEqual([name for name, _ in self.actions], ["wineboot", "wineserver", "wine"])
        self.assertTrue(all(prefix == self.prefix for _, _, prefix in self.calls))

    def test_failed_shutdown_never_launches_or_forces_exit(self):
        for failure in ("wineboot", "wineserver", "timeout:wineboot", "timeout:wineserver"):
            with self.subTest(failure=failure):
                result = self.launch("--recover-display", MOCK_FAIL=failure)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("retry", result.stderr)
                self.assertNotIn("wine", [name for name, _ in self.actions])
                if failure.endswith("wineboot"):
                    self.assertNotIn("wineserver", [name for name, _ in self.actions])
                for _, args, _ in self.calls:
                    self.assertFalse({"--kill", "--force", "-k", "-f"}.intersection(args))

    def test_recovery_does_not_install_a_missing_application(self):
        result = self.launch("--recover-display", MOCK_MISSING="1")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("not installed", result.stderr)
        self.assertEqual([name for name, _, _ in self.calls], ["ls"])

    def test_extra_recovery_arguments_do_not_restart(self):
        # This path exits before it needs any external command.
        self.log.touch()
        result = subprocess.run(["/bin/bash", str(LAUNCHER), "--recover-display", "extra"],
                                env=self.env, text=True, capture_output=True, timeout=10)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("Usage:", result.stderr)
        self.assertEqual(self.log.read_text(), "")


if __name__ == "__main__":
    unittest.main(verbosity=2)
