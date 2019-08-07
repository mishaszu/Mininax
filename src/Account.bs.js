// Generated by BUCKLESCRIPT VERSION 5.0.4, PLEASE EDIT WITH CARE
'use strict';

var React = require("react");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Utils$ReactHooksTemplate = require("./Utils.bs.js");
var Configs$ReactHooksTemplate = require("./configs.bs.js");
var Styles1$ReactHooksTemplate = require("./Styles1.bs.js");
var ItemDisplay$ReactHooksTemplate = require("./ItemDisplay.bs.js");
var ContextProvider$ReactHooksTemplate = require("./ContextProvider.bs.js");

function Account(Props) {
  var items = Props.items;
  var goToDetail = Props.goToDetail;
  var theme = React.useContext(ContextProvider$ReactHooksTemplate.themeContext);
  var changeLevel = function (level) {
    console.log("level", level);
    return /* () */0;
  };
  var id = Utils$ReactHooksTemplate.getValueFromDict(items, "account_id");
  var displayName = Utils$ReactHooksTemplate.getDisplayName(Caml_array.caml_array_get(Configs$ReactHooksTemplate.configs, theme));
  var onOpenAccountSends = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForAccountSends(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  var onOpenAccountReceipts = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForAccountReceipts(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  var onOpenAccountOtherOperations = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForOtherOperations(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  var onOpenAccountEndorsements = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForEndorsements(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  var onOpenAccountBakedBlocks = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForBakedBlocks(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  var onOpenAccountDeposits = function (_ev) {
    var query = Utils$ReactHooksTemplate.getQueryForDepositsAndRewards(id);
    return Utils$ReactHooksTemplate.openSharedUrl(query, displayName, "operations");
  };
  return React.createElement("div", {
              className: Styles1$ReactHooksTemplate.container(theme)
            }, React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.title(theme)
                }, "Account"), React.createElement(ItemDisplay$ReactHooksTemplate.make, {
                  entity: "account",
                  items: items,
                  changeLevel: changeLevel,
                  goToDetail: goToDetail
                }), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountSends
                }, "All sent transactions"), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountReceipts
                }, "All received transactions"), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountOtherOperations
                }, "All reveals, delegations and originations"), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountEndorsements
                }, "All endorsements"), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountBakedBlocks
                }, "All baked blocks"), React.createElement("div", {
                  className: Styles1$ReactHooksTemplate.accountBtn(theme),
                  onClick: onOpenAccountDeposits
                }, "All deposits and rewards"));
}

var make = Account;

exports.make = make;
/* react Not a pure module */