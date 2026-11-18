#!/usr/bin/env python3
"""Add AppsFlyer RPC + Framework SwiftPM refs to a CMake-generated iOS app xcodeproj."""
from __future__ import annotations

import json
import re
import sys
import uuid
from pathlib import Path


def stable_id(seed: str) -> str:
    return uuid.uuid5(uuid.NAMESPACE_DNS, f"appsflyer.cocos2dx.spm.{seed}").hex[:24].upper()


def load_versions(repo_root: Path) -> dict:
    path = repo_root / "version/ios-spm-versions.json"
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def strip_stale_pod_build_phases(text: str) -> str:
    """Remove leftover [CP] / Pods_* references if an old pbxproj still contains them."""
    text = re.sub(
        r"\n\t\t[0-9A-F]{24} /\* \[CP\][^\n]*\*/,?",
        "",
        text,
    )
    text = re.sub(r"\n\t\t[A-F0-9]{24} /\* Pods_[^\n]*\*/,?", "", text)
    text = re.sub(
        r"\n\t\t[A-F0-9]{24} /\* Pods-AppsFlyerCocos2dX[^\n]*\*/,?",
        "",
        text,
    )
    text = re.sub(
        r"\t+[A-F0-9]{24} /\* Pods-AppsFlyerCocos2dX[^\n]*\n",
        "",
        text,
    )
    text = re.sub(
        r"\n\t\tbaseConfigurationReference = [A-F0-9]{24} /\* Pods-AppsFlyerCocos2dX[^\n]*\n",
        "\n",
        text,
    )
    return text


def already_has_spm(text: str) -> bool:
    return "appsflyer-apple-rpc" in text


def strip_legacy_build_locations(text: str) -> str:
    """CMake Xcode generator sets SYMROOT/CONFIGURATION_BUILD_DIR; SPM requires default locations."""
    text = re.sub(r"\n\t\t\t\tSYMROOT = [^;]+;", "", text)
    text = re.sub(
        r"\n\t\t\t\tCONFIGURATION_BUILD_DIR = [^;]*bin/AppsFlyerCocos2dX/[^;]+;",
        "",
        text,
    )
    text = re.sub(
        r"\n\t\t\t\tTARGET_TEMP_DIR = [^;]*AppsFlyerCocos2dX\.build/[^;]+;",
        "",
        text,
    )
    return text


WORKSPACE_SETTINGS = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
\t<key>BuildLocationStyle</key>
\t<string>UseAppPreferences</string>
\t<key>DerivedDataLocationStyle</key>
\t<string>Default</string>
\t<key>IssueFilterStyle</key>
\t<string>ShowActiveSchemeOnly</string>
\t<key>LiveSourceIssuesEnabled</key>
\t<true/>
\t<key>ShowSharedSchemesAutomaticallyEnabled</key>
\t<true/>
</dict>
</plist>
"""


def ensure_workspace_build_settings(xcodeproj: Path) -> None:
    ws = xcodeproj / "project.xcworkspace/xcshareddata"
    ws.mkdir(parents=True, exist_ok=True)
    settings = ws / "WorkspaceSettings.xcsettings"
    if not settings.is_file():
        settings.write_text(WORKSPACE_SETTINGS, encoding="utf-8")


def find_app_target(text: str) -> re.Match[str] | None:
    return re.search(
        r"([A-F0-9]{24}) /\* AppsFlyerCocos2dX \*/ = \{.*?productType = \"com\.apple\.product-type\.application\";\n\t\t\};",
        text,
        re.DOTALL,
    )


def inject(text: str, versions: dict) -> str:
    rpc_ver = versions["appsflyerAppleRpc"]
    fw_ver = versions["appsflyerFrameworkStatic"]
    rpc_url = versions["appsflyerAppleRpcUrl"]
    fw_url = versions["appsflyerFrameworkStaticUrl"]
    rpc_product = versions["productRpc"]
    fw_product = versions["productFramework"]

    ref_rpc = stable_id("ref-rpc")
    ref_fw = stable_id("ref-fw")
    dep_rpc = stable_id("dep-rpc")
    dep_fw = stable_id("dep-fw")

    spm_blocks = f"""
/* Begin XCRemoteSwiftPackageReference section */
\t\t{ref_rpc} /* XCRemoteSwiftPackageReference "appsflyer-apple-rpc" */ = {{
\t\t\tisa = XCRemoteSwiftPackageReference;
\t\t\trepositoryURL = "{rpc_url}";
\t\t\trequirement = {{
\t\t\t\tkind = exactVersion;
\t\t\t\tversion = {rpc_ver};
\t\t\t}};
\t\t}};
\t\t{ref_fw} /* XCRemoteSwiftPackageReference "AppsFlyerFramework-Static" */ = {{
\t\t\tisa = XCRemoteSwiftPackageReference;
\t\t\trepositoryURL = "{fw_url}";
\t\t\trequirement = {{
\t\t\t\tkind = exactVersion;
\t\t\t\tversion = {fw_ver};
\t\t\t}};
\t\t}};
/* End XCRemoteSwiftPackageReference section */

/* Begin XCSwiftPackageProductDependency section */
\t\t{dep_rpc} /* {rpc_product} */ = {{
\t\t\tisa = XCSwiftPackageProductDependency;
\t\t\tpackage = {ref_rpc} /* XCRemoteSwiftPackageReference "appsflyer-apple-rpc" */;
\t\t\tproductName = {rpc_product};
\t\t}};
\t\t{dep_fw} /* {fw_product} */ = {{
\t\t\tisa = XCSwiftPackageProductDependency;
\t\t\tpackage = {ref_fw} /* XCRemoteSwiftPackageReference "AppsFlyerFramework-Static" */;
\t\t\tproductName = {fw_product};
\t\t}};
/* End XCSwiftPackageProductDependency section */
"""

    marker = "/* End PBXProject section */"
    if marker not in text:
        raise SystemExit("PBXProject section not found")
    text = text.replace(marker, spm_blocks + "\n" + marker)

    if "packageReferences" not in text or "appsflyer-apple-rpc" not in text:
        proj_pat = r"(/\* Project object \*/ = \{.*?)(\n\t\t\ttargets = \()"
        match = re.search(proj_pat, text, re.DOTALL)
        if not match:
            raise SystemExit("PBXProject object not found")
        injection = (
            f"\n\t\t\tpackageReferences = (\n"
            f"\t\t\t\t{ref_rpc} /* XCRemoteSwiftPackageReference \"appsflyer-apple-rpc\" */,\n"
            f"\t\t\t\t{ref_fw} /* XCRemoteSwiftPackageReference \"AppsFlyerFramework-Static\" */,\n"
            f"\t\t\t);"
        )
        text = text[: match.start()] + match.group(1) + injection + match.group(2) + text[match.end() :]

    app_target = find_app_target(text)
    if not app_target:
        raise SystemExit('AppsFlyerCocos2dX app target not found')
    tblock = app_target.group(0)
    if "packageProductDependencies" not in tblock:
        insert = (
            f"\t\t\tpackageProductDependencies = (\n"
            f"\t\t\t\t{dep_rpc} /* {rpc_product} */,\n"
            f"\t\t\t\t{dep_fw} /* {fw_product} */,\n"
            f"\t\t\t);\n"
        )
        tblock = tblock.replace(
            "\t\t\tproductType = \"com.apple.product-type.application\";",
            insert + "\t\t\tproductType = \"com.apple.product-type.application\";",
        )
        text = text.replace(app_target.group(0), tblock)

    min_ios = versions.get("cmakeDeploymentTarget") or versions.get("minimumIos", "15.0")
    text = re.sub(
        r"(IPHONEOS_DEPLOYMENT_TARGET = )[0-9.]+;",
        rf"\g<1>{min_ios};",
        text,
    )
    return strip_legacy_build_locations(text)


def main() -> int:
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <path/to/AppsFlyerCocos2dX.xcodeproj>", file=sys.stderr)
        return 2
    xcodeproj = Path(sys.argv[1]).resolve()
    pbx = xcodeproj / "project.pbxproj"
    if not pbx.is_file():
        print(f"Missing {pbx}", file=sys.stderr)
        return 1
    repo_root = Path(__file__).resolve().parents[2]
    versions = load_versions(repo_root)
    text = pbx.read_text(encoding="utf-8")
    changed = False
    if already_has_spm(text):
        print("SwiftPM packages already present — applying SPM-compatible build locations only")
        patched = strip_legacy_build_locations(text)
        if patched != text:
            text = patched
            changed = True
    else:
        text = strip_stale_pod_build_phases(text)
        text = inject(text, versions)
        changed = True
    ensure_workspace_build_settings(xcodeproj)
    if changed:
        pbx.write_text(text, encoding="utf-8")
        print("Updated", pbx)
    else:
        print("No project.pbxproj changes needed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())