open MainType;

[@bs.val] external btoa : string => string = "window.btoa";
[@bs.val] external encodeURIComponent : string => string = "encodeURIComponent";
[@bs.scope "window"] [@bs.val] external open_ : (string, string) => unit = "open";

let arronaxURL = "https://arronax.io/";
let invalidId = "You entered an invalid ID.";
let missingContract = "Contract does not exist";
let missingAccount = "Account does not exist or has 0 balance";
let invalidAccountId = "You entered an invalid account hash";
let operationNotFound = "Operation not found or not yet recorded. Check back in a bit.";
let noBlock = "There is no block at this level.";
let noAvailable = "Not available";

let getInfo = (config: MainType.config) => {
  let conseilServerInfo: MainType.conseilServerInfo = [%bs.obj {url: config.url, apiKey: config.apiKey, network: config.network}];
  (conseilServerInfo, config.platform, config.network);
};

let getDisplayName = (config: MainType.config) => config.displayName;

let convertNumberByIntl: (bool, int, int, float ) => string = [%bs.raw {|
    function (useGrouping, minimumFractionDigits, maximumFractionDigits, value) {
      return new Intl.NumberFormat(window.navigator.languages[0], { style: 'decimal', useGrouping, minimumFractionDigits, maximumFractionDigits }).format(value);
    }
  |}];

let convertFromUtezToTez = (utez: int) => float_of_int(utez) /. 1000000.;
let convertFromUtezfToTez = (utez: float) => utez /. 1000000.;

let formatNumber = (value, isScale) => {
  let realVal = Js.Nullable.toOption(value);
  switch (realVal, isScale) {
  | (Some(val_), true) => {
    let d = convertFromUtezToTez(val_);
    let (minimumFractionDigits, maximumFractionDigits) =
      if (val_ < 10000) {
        (6, 6);
      } else if (val_ < 100000) {
        (4, 4);
      } else {
        (2, 2);
      };
      convertNumberByIntl(true, minimumFractionDigits, maximumFractionDigits, d);
  }
  | (Some(val_), false) => convertNumberByIntl(false, 0, 1, float_of_int(val_))
  | _ => ""
  };
};

let formatString = (value, isConvert) => {
  let realVal = Js.Nullable.toOption(value);
  switch (realVal, isConvert) {
    | (Some(val_), true) => {
      val_ |> Js.String.split("_") |> Js.Array.map(s => Js.String.toUpperCase(Js.String.charAt(0, s)) ++ Js.String.slice(~from=1, ~to_= Js.String.length(s), s)) |> Js.Array.joinWith(" ");
    }
    | (Some(val_), false) => val_
    | _ => ""
  };
};

let makeUrl = (network: string, entity: string, id: string) => "/" ++ network ++ "/" ++ entity ++ "/" ++ id;

let getFields = (~entity, ~kind=?, ()): array(MainType.field) => {
  switch (entity, kind) {
    | ("operation", Some("Transaction")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "From", isLink: true, showNotifierLink: true},
        {name: "destination", displayName: "To", isLink: true, showNotifierLink: true},
        {name: "amount", displayName: "Amount", isLink: false, showNotifierLink: false},
        {name: "fee", displayName: "Fee", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "parameters", displayName: "Parameters", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Activate Account")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "pkh", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "secret", displayName: "Secret", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Reveal")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "public_key", displayName: "Public Key", isLink: false, showNotifierLink: false},
        {name: "fee", displayName: "Fee", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Delegation")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "delegate", displayName: "Delegate", isLink: false, showNotifierLink: false},
        {name: "fee", displayName: "Fee", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Origination")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "delegate", displayName: "Delegate", isLink: false, showNotifierLink: false},
        {name: "balance", displayName: "Balance", isLink: false, showNotifierLink: false},
        {name: "fee", displayName: "Fee", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "originated_contracts", displayName: "Originated Contracts", isLink: true, showNotifierLink: false},
        {name: "script", displayName: "Script", isLink: false, showNotifierLink: false},
        {name: "storage", displayName: "Storage", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Ballot")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "proposal", displayName: "Proposal", isLink: false, showNotifierLink: false},
        {name: "ballot", displayName: "Vote", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Seed Nonce Revelation")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "nonce", displayName: "Nonce", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Proposals")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "source", displayName: "Account ID", isLink: true, showNotifierLink: true},
        {name: "proposal", displayName: "Proposal", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Gas", isLink: false, showNotifierLink: false},
        {name: "status", displayName: "Status", isLink: false, showNotifierLink: false}
      |]
    | ("operation", Some("Endorsement")) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "delegate", displayName: "Delegate", isLink: true, showNotifierLink: true},
        {name: "slots", displayName: "Slots", isLink: false, showNotifierLink: false}
      |]
    | ("operation", _) => [|
        {name: "operation_group_hash", displayName: "Operation Hash", isLink: false, showNotifierLink: false},
        {name: "kind", displayName: "Kind", isLink: false, showNotifierLink: false},
        {name: "block_hash", displayName: "Block Hash", isLink: true, showNotifierLink: false}
      |]
    | ("block", None) => [|
        {name: "hash", displayName: "Hash", isLink: false, showNotifierLink: false},
        {name: "predecessor", displayName: "Predecessor", isLink: true, showNotifierLink: false},
        {name: "level", displayName: "Level", isLink: false, showNotifierLink: false},
        {name: "timestamp", displayName: "Timestamp", isLink: false, showNotifierLink: false},
        {name: "chain_id", displayName: "Chain ID", isLink: false, showNotifierLink: false},
        {name: "protocol", displayName: "Protocol", isLink: false, showNotifierLink: false},
        {name: "consumed_gas", displayName: "Consumed Gas", isLink: false, showNotifierLink: false},
        {name: "total_amount", displayName: "Total Transacted", isLink: false, showNotifierLink: false},
        {name: "fee", displayName: "Total Fees", isLink: false, showNotifierLink: false},
        {name: "baker", displayName: "Baker", isLink: true, showNotifierLink: true},
        {name: "baker_priority", displayName: "Baker Priority", isLink: false, showNotifierLink: false},
        /* {name: "meta_cycle", displayName: "Cycle", isLink: false, showNotifierLink: false}, */
        {name: "meta_cycle_position", displayName: "Cycle Position", isLink: false, showNotifierLink: false},
        {name: "period_kind", displayName: "Governance Period", isLink: false, showNotifierLink: false},
        {name: "active_proposal", displayName: "Active Proposal", isLink: false, showNotifierLink: false},
        {name: "signature", displayName: "Signature", isLink: false, showNotifierLink: false}
      |]
    | ("account", None) => [|
        {name: "account_id", displayName: "Address", isLink: false, showNotifierLink: true},
        {name: "block_id", displayName: "Last Updated", isLink: true, showNotifierLink: false},
        {name: "balance", displayName: "Balance", isLink: false, showNotifierLink: false},
        {name: "manager", displayName: "Manager", isLink: true, showNotifierLink: false},
        {name: "delegate_value", displayName: "Delegate", isLink: true, showNotifierLink: false},
        {name: "script", displayName: "Script", isLink: false, showNotifierLink: false},
        {name: "storage", displayName: "Storage", isLink: false, showNotifierLink: false},
        {name: "baker_deactivated", displayName: "Active Baker", isLink: false, showNotifierLink: false},
        {name: "baker_balance", displayName: "Snapshot Balance", isLink: false, showNotifierLink: false},
        {name: "baker_delegated_balance", displayName: "Delegated Balance", isLink: false, showNotifierLink: false},
        {name: "baker_frozen_balance", displayName: "Frozen Balance", isLink: false, showNotifierLink: false},
        {name: "baker_staking_balance", displayName: "Staking Balance", isLink: false, showNotifierLink: false},
      |]
    | _ => [||]
  };
};

// at the basic level we could reduce a bit code here by applying records to set, it's also a bit more descriptive. Mind we're mutating data here.
let applyProperties = (block, ~field, ~value) => {
  Js.Dict.set(block, field, value);
  block;
}

let convertBlock = (~block, ~total=?, ()) => {
  let assBlock = Js.Obj.assign (Js.Obj.empty(), block);
  let newBlock = Js.Dict.empty()
  |> applyProperties(~field="hash", ~value=formatString(assBlock##hash, false))
  |> applyProperties(~field="hash", ~value=formatString(assBlock##hash, false))
  |> applyProperties(~field="predecessor", ~value=formatString(assBlock##predecessor, false))
  |> applyProperties(~field="level", ~value=assBlock##level |> string_of_int)
  |> applyProperties(~field="meta_voting_period", ~value=assBlock##meta_voting_period |> string_of_int)
  |> applyProperties(~field="timestamp", ~value=assBlock##timestamp |> string_of_int)
  |> applyProperties(~field="chain_id", ~value=formatString(assBlock##chain_id, true))
  |> applyProperties(~field="protocol", ~value=formatString(assBlock##protocol, false))
  |> applyProperties(~field="consumed_gas", ~value=formatNumber(assBlock##consumed_gas, false))
  |> applyProperties(~field="baker", ~value=formatString(assBlock##baker, false))
  |> applyProperties(~field="baker_priority", ~value=formatNumber(assBlock##priority, false))
  |> applyProperties(~field="meta_cycle", ~value=assBlock##meta_cycle |> string_of_int)
  |> applyProperties(~field="meta_cycle_position", ~value=assBlock##meta_cycle_position |> string_of_int)
  |> applyProperties(~field="period_kind", ~value=formatString(assBlock##period_kind, true))
  |> applyProperties(~field="active_proposal", ~value=formatString(assBlock##active_proposal, true))
  |> applyProperties(~field="signature", ~value=formatString(assBlock##signature, false));
  switch (total) {
  | None => {
    Js.Dict.set(newBlock, "total_amount", "");
    Js.Dict.set(newBlock, "fee", "");
    newBlock;
  }
  | Some(tt) => {
    let assTotal = Js.Obj.assign (Js.Obj.empty(), tt);
    Js.Dict.set(newBlock, "total_amount", formatNumber(assTotal##sum_amount, true));
    Js.Dict.set(newBlock, "fee", formatNumber(assTotal##sum_fee, true));
    newBlock;
  }}
};

let convertOperation = (operation) => {
  let assOp = Js.Obj.assign (Js.Obj.empty(), operation);
  let newOp = Js.Dict.empty();
  Js.Dict.set(newOp, "operation_group_hash", formatString(assOp##operation_group_hash, false));
  Js.Dict.set(newOp, "kind", formatString(assOp##kind, true));
  Js.Dict.set(newOp, "level", assOp##level |> string_of_int);
  Js.Dict.set(newOp, "block_hash", formatString(assOp##block_hash, false));
  Js.Dict.set(newOp, "timestamp", assOp##timestamp |> string_of_int);
  switch assOp##kind {
    | "transaction" => {
      Js.Dict.set(newOp, "source", formatString(assOp##source, false));
      Js.Dict.set(newOp, "destination", formatString(assOp##destination, false));
      Js.Dict.set(newOp, "amount", formatNumber(assOp##amount, true));
      Js.Dict.set(newOp, "fee", formatNumber(assOp##fee, true));
      Js.Dict.set(newOp, "consumed_gas", formatNumber(assOp##consumed_gas, false));
      Js.Dict.set(newOp, "parameters", formatString(assOp##parameters, false));
      Js.Dict.set(newOp, "status", formatString(assOp##status, true));
      newOp;
    }
    | "activate_account" => {
      Js.Dict.set(newOp, "pkh", formatString(assOp##pkh, false));
      Js.Dict.set(newOp, "secret", formatString(assOp##secret, false));
      newOp;
    }
    | "ballot" => {
      Js.Dict.set(newOp, "proposal", formatString(assOp##proposal, false));
      Js.Dict.set(newOp, "ballot", formatString(assOp##ballot, true));
      newOp;
    }
    | "proposals" => {
      Js.Dict.set(newOp, "proposal", formatString(assOp##proposal, false));
      newOp;
    }
    | "seed_nonce_revelation" => {
      Js.Dict.set(newOp, "nonce", formatString(assOp##nonce, false));
      newOp;
    }
    | "reveal" => {
      Js.Dict.set(newOp, "source", formatString(assOp##source, false));
      Js.Dict.set(newOp, "public_key", formatString(assOp##public_key, false));
      Js.Dict.set(newOp, "fee", formatNumber(assOp##fee, true));
      Js.Dict.set(newOp, "consumed_gas", formatNumber(assOp##consumed_gas, false));
      Js.Dict.set(newOp, "status", formatString(assOp##status, true));
      newOp;
    }
    | "delegation" => {
      Js.Dict.set(newOp, "source", formatString(assOp##source, false));
      Js.Dict.set(newOp, "delegate", assOp##delegate);
      Js.Dict.set(newOp, "fee", formatNumber(assOp##fee, true));
      Js.Dict.set(newOp, "consumed_gas", formatNumber(assOp##consumed_gas, false));
      Js.Dict.set(newOp, "status", formatString(assOp##status, true));
      newOp;
    }
    | "origination" => {
      Js.Dict.set(newOp, "source", formatString(assOp##source, false));
      Js.Dict.set(newOp, "delegate", formatString(assOp##delegate, false));
      Js.Dict.set(newOp, "balance", formatNumber(assOp##balance, true));
      Js.Dict.set(newOp, "fee", formatNumber(assOp##fee, true));
      Js.Dict.set(newOp, "consumed_gas", formatNumber(assOp##consumed_gas, false));
      Js.Dict.set(newOp, "originated_contracts", formatString(assOp##originated_contracts, false));
      Js.Dict.set(newOp, "script", formatString(assOp##script, false));
      Js.Dict.set(newOp, "storage", formatString(assOp##storage, false));
      Js.Dict.set(newOp, "status", formatString(assOp##status, true));
      newOp;
    }
    | "endorsement" => {
      Js.Dict.set(newOp, "delegate", formatString(assOp##delegate, false));
      Js.Dict.set(newOp, "slots", formatString(assOp##slots, false));
      newOp;
    }
    | _ => newOp
  };
};

let convertAccount = (~account, ~baker=?, ()) => {
  let assAccount = Js.Obj.assign (Js.Obj.empty(), account);
  let newAccount = Js.Dict.empty();
  Js.Dict.set(newAccount, "account_id", formatString(assAccount##account_id, false));
  Js.Dict.set(newAccount, "block_id", formatString(assAccount##block_id, false));
  Js.Dict.set(newAccount, "manager", formatString(assAccount##manager, false));
  Js.Dict.set(newAccount, "delegate_value", formatString(assAccount##delegate_value, false));
  Js.Dict.set(newAccount, "script", formatString(assAccount##script, false));
  Js.Dict.set(newAccount, "storage", formatString(assAccount##storage, false));
  Js.Dict.set(newAccount, "balance", formatNumber(assAccount##balance, true));
  switch (baker) {
  | None => {
    Js.Dict.set(newAccount, "baker_deactivated", "No");
    Js.Dict.set(newAccount, "baker_balance", "");
    Js.Dict.set(newAccount, "baker_delegated_balance", "");
    Js.Dict.set(newAccount, "baker_frozen_balance", "");
    Js.Dict.set(newAccount, "baker_staking_balance", "");
    newAccount;
  }
  | Some(tt) => {
    let assBaker = Js.Obj.assign (Js.Obj.empty(), tt);
    Js.Dict.set(newAccount, "baker_deactivated", assBaker##deactivated ? "No" : "Yes");
    Js.Dict.set(newAccount, "baker_balance", formatNumber(assBaker##balance , true));
    Js.Dict.set(newAccount, "baker_delegated_balance", formatNumber(assBaker##delegated_balance, true));
    Js.Dict.set(newAccount, "baker_frozen_balance", formatNumber(assBaker##frozen_balance, true));
    Js.Dict.set(newAccount, "baker_staking_balance", formatNumber(assBaker##staking_balance, true));
    newAccount;
  }}
};

let getSecondTimeFromMilli = (nowTime: float) => int_of_float(nowTime /. 1000.);

let isNumber = (id: string) => {
  switch (int_of_string(id)) {
    | exception _ => false
    | _ => true
  };
};

let getValueFromDict = (dict: Js.Dict.t(string), key_: string) => {
  switch (Js.Dict.get(dict, key_)) {
    | None => ""
    | Some(value) => value
  };
};

let copyContent: (string) => unit = [%bs.raw {|
    function (value) {
      const textField = document.createElement("textarea");
      textField.innerText = value;
      document.body.appendChild(textField);
      textField.select();
      document.execCommand("copy");
      textField.remove();
    }
  |}];

let jsonConvertQuery: (ConseiljsType.conseilQuery) => string = [%bs.raw {|
    function (value) {
      return JSON.stringify(value)
    }
  |}];

let openSharedUrl = (query: ConseiljsType.conseilQuery, displayName: string, entity: string) => {
  let serializedQuery = jsonConvertQuery(query);
  let encodedUrl = btoa(serializedQuery);
  let shareLink = arronaxURL ++ "?e=" ++ encodeURIComponent(displayName) ++ "/" ++ encodeURIComponent(entity) ++ "&q=" ++ encodedUrl;
  open_(shareLink, "_blank");
};

let getBlocksPerCycle = (network: string) => 
  switch network {
  | "mainnet" => 4096
  | "babylonnet" => 2048
  | "carthagenet" => 2048
  | "zeronet" => 128
  | _ => 0
  };
